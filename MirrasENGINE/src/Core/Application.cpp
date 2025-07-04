#include "Core/Application.h"

#include "Core/Timer.h"
#include "Core/Log.h"
#include "Core/Asserts.h"

#include "Events/WindowEvents.h"
#include "Events/MouseEvents.h"

#include "Input/Input.h"

#include "UI/ImGui.h"

#include <ranges>
#include <filesystem_fs>

namespace mirras
{
    App::App(const AppSpecs& appSpecs, const WindowSpecs& windowSpecs)
    {
        MIRR_ASSERT_CORE(appInstance == nullptr, "One instance of application already exists");
        appInstance = this;

        if(!appSpecs.workingDirectory.empty())
            fs::current_path(appSpecs.workingDirectory);

        window.init(windowSpecs);

        Log::initAppLog(appSpecs.name);

        Renderer::init(appSpecs.backend);
        Renderer::setClearColor(0.2f, 0.2f, 0.2f);

        imgui::init();
        
        window.setOnEventCallback([this](Event& event)
        {
            App::onEvent(event);
        });

        fixedTimestep = 1.f / appSpecs.updateRate;
    }

    void App::run()
    {
        window.makeVisible(true);

        ASYNC_UPDATE
        (
            window.makeContextCurrent(false);
            updateThread = std::thread{&App::update, this};
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

                {
                    std::lock_guard lock{layersMutex};

                    updateLayers(frameTime);

                    renderLayers();

                    Input::mouseWheelScroll = vec2f{};
                }

                if(resizing)
                {
                    synchronizeResize();

                    // Because of the way we do multithreading, ImGui widgets start to flicker on resize when VSync is disabled
                    // Waiting here for atleast 4ms seems to eliminate the flickering for the most part (have to test elsewhere)
                    if(!OSWindow::isVSynced())
                        wait(4_ms);

                    continue;
                }

                window.swapBuffers();
            }
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

    void App::onWindowClose(WindowClosed& event)
    {
        ENGINE_LOG_INFO("Window Close Event");
        running = false;
    }

    static void updateMouseScrollInput(MouseWheelScrolled& event)
    {
        Input::mouseWheelScroll = event.mouseWheelOffset;
    }

    void App::onEvent(Event& event)
    {
        Event::dispatch_to_member<WindowClosed, &App::onWindowClose>(event, this);
        Event::dispatch_to_member<WindowResized, &App::onWindowResize>(event, this);

        imgui::onEvent(event);

        {
            ASYNC_UPDATE (
                std::lock_guard lock{layersMutex};
            )

            Event::dispatch<MouseWheelScrolled, &updateMouseScrollInput>(event);

            for(auto& layer : layers | std::views::reverse)
            {
                if(!event.propagable)
                    break;

                layer->onEvent(event);
            }
        }

        // Another way to dispatch events, not limited to functions with only an event as argument
        /*if(Event::is_a<WindowResized>(event))
            onWindowResize(static_cast<WindowResize&>(event));
        else
        if(Event::is_a<WindowClosed>(event))
            onWindowClose(static_cast<WindowClose&>(event));*/
    }

    App& App::getInstance()
    {
        MIRR_ASSERT_CORE(appInstance, "No application instance yet");
        return *appInstance;
    }

    void App::addLayer(single_ref<Layer> layer)
    {
        ASYNC_UPDATE (
            std::lock_guard lock{layersMutex};
        )

        layers.addLayer(std::move(layer));
    }

    void App::addOverlay(single_ref<Layer> layer)
    {
        ASYNC_UPDATE (
            std::lock_guard lock{layersMutex};
        )
    
        layers.addOverlay(std::move(layer));
    }

    App::~App()
    {
        running = false;

        ASYNC_UPDATE
        (
            if(updateThread.joinable())
                updateThread.join();
        )

        imgui::shutdown();
        Renderer::shutdown();
    }
} // namespace mirras
