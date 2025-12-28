#include "Core/Application.h"

#include "Core/Timer.h"
#include "Core/Log.h"
#include "Core/Asserts.h"

#include "Events/WindowEvents.h"
#include "Events/MouseEvents.h"

#include "Scripting/ScriptManager.h"
#include "Input/Input.h"
#include "UI/ImGui.h"

#include <ranges>
#include <cstdlib>
#include <filesystem_fs>

#ifdef RUN_UPDATE_THREAD
    #include <thread>
#endif

namespace mirras
{
    App::App(const AppSpecs& appSpecs, const WindowSpecs& windowSpecs)
    {
        MIRR_ASSERT_CORE(appInstance == nullptr, "One instance of application already exists");
        appInstance = this;

        eventHandler.connectEvents(this);

        if(!appSpecs.workingDirectory.empty())
            fs::current_path(appSpecs.workingDirectory);

        window.init(windowSpecs);
        window.setAppEventHandler(&eventHandler);

        Log::initAppLog(appSpecs.name);

        Renderer::init(appSpecs.backend);
        Renderer::setClearColor(0.2f, 0.2f, 0.2f);

        imgui::init();
        ScriptManager::init();

        fixedTimestep = 1.f / appSpecs.fixedUpdateRate;
        handleStopOnClose = appSpecs.autoStopOnClose;

        std::atexit(Renderer::shutdown);
    }

    void App::run()
    {
        window.makeVisible(true);

        ASYNC_UPDATE
        (
            window.makeContextCurrent(false);
            std::jthread updateThread{&App::update, this};
        )
        
        Timer timer;

        while(running)
        {
            ASYNC_UPDATE (
                window.waitEvents();
            )

            NO_ASYNC_UPDATE
            (
                float frameTime = timer.elapsed();

                window.pollEvents();

                updateLayers(frameTime);

                renderLayers();

                window.swapBuffers();

                Input::mouseWheelScroll = vec2f{};
            )
        }
    }

    void App::stop()
    {
        running = false;
        window.postEmptyEvent();
    }

    void App::updateLayers(float frameTime)
    {
        static float frameTimeAccumulator{};
        frameTimeAccumulator += frameTime;

        while(frameTimeAccumulator >= fixedTimestep)
        {
            for(auto& layer : layers)
                layer->fixedUpdate(fixedTimestep);

            frameTimeAccumulator -= fixedTimestep;
        }

        for(auto& layer : layers)
            layer->update(frameTime);
    }

    void App::renderLayers()
    {
        Renderer::clearBackBuffers();

        Renderer::beginDrawing();

            for(auto& layer : layers)
                layer->draw();

        Renderer::endDrawing();

        imgui::beginFrame();

            for(auto& layer : layers)
                layer->drawImGui();

        imgui::endFrame();
    }

    ASYNC_UPDATE
    (
        void App::synchronizeResize()
        {
            window.makeContextCurrent(false);

            switchContext = true;
            switchContext.notify_one();

            resizing.wait(true); // Wait until we are notified that 'resizing' was set to false

            switchContext = false;
            window.makeContextCurrent(true);
        }
        
        void App::update()
        {
            window.makeContextCurrent(true);
            Timer timer;

            while(running)
            {
                float frameTime = timer.elapsed();

                eventHandler.process();

                updateLayers(frameTime);

                if(resizing)
                {
                    synchronizeResize();

                    // Because of the way we do multithreading, ImGui widgets start to flicker on resize when VSync is disabled
                    // Waiting here for atleast 4ms seems to eliminate the flickering for the most part (have to test elsewhere)

                    // Also, while testing the engine with lower CPU clock speeds (notebook unplugged), there's some flickering
                    // when running on release even with VSync enabled. So it's better to slowdown a bit, regardless of configuration

                    //if(!OSWindow::isVSynced())
                        wait(4_ms);

                    continue;
                }

                renderLayers();

                Input::mouseWheelScroll = vec2f{};

                window.swapBuffers();
            }

            window.makeContextCurrent(false);
        }

        void App::handleResize(int32 width, int32 height)
        {
            resizing = true;

            switchContext.wait(false); // Wait until we are notified that 'switchContext' was set to true

            window.makeContextCurrent(true);

            Renderer::setWindowViewport(0, 0, width, height);
            
            // Keep rendering on window resize
            renderLayers();

            window.swapBuffers();

            window.makeContextCurrent(false);

            resizing = false;
            resizing.notify_one();
        }
    )

    void App::onWindowResize(WindowResized& event)
    {
        auto [width, height] = event.windowSize;

        if(width == 0 || height == 0)
            return;

        ASYNC_UPDATE (
            handleResize(width, height);
        )

        NO_ASYNC_UPDATE (
            Renderer::setWindowViewport(0, 0, width, height);
        )
    }

    static void updateMouseScrollInput(MouseWheelScrolled& event)
    {
        Input::mouseWheelScroll = event.mouseWheelOffset;
    }

    void App::onEvent(Event& event)
    {
        if(Event::is_a<WindowClosed>(event) && handleStopOnClose)
            stop();

        imgui::onEvent(event);

        Event::dispatch<MouseWheelScrolled, &updateMouseScrollInput>(event);

        for(auto& layer : layers | std::views::reverse)
        {
            if(!event.propagable)
                break;

            layer->onEvent(event);
        }
    }

    App& App::getInstance()
    {
        MIRR_ASSERT_CORE(appInstance, "No application instance yet");
        return *appInstance;
    }

    void App::addLayer(single_ref<Layer> layer)
    {
        layers.addLayer(std::move(layer));
    }

    void App::addOverlay(single_ref<Layer> layer)
    {
        layers.addOverlay(std::move(layer));
    }

    App::~App()
    {
        ASYNC_UPDATE (
            window.makeContextCurrent(true);
        )

        imgui::shutdown();
    }
} // namespace mirras
