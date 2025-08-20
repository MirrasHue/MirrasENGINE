#include "Core/Application.h"

#include "Core/Timer.h"
#include "Core/Log.h"
#include "Core/Asserts.h"

#include "Events/WindowEvents.h"
#include "Events/MouseEvents.h"

#include "Input/Input.h"

#include <ranges>
#include <filesystem_fs>

#ifdef RUN_UPDATE_THREAD
    #include <thread>
#endif

namespace mirras
{
    App::App(const AppSpecs& appSpecs, const WindowSpecs& windowSpecs)
    {
        MIRR_ASSERT(appInstance == nullptr, "One instance of application already exists");
        appInstance = this;

        eventHandler.connectEvents(this);

        if(!appSpecs.workingDirectory.empty())
            fs::current_path(appSpecs.workingDirectory);

        window.init(windowSpecs);
        window.setAppEventHandler(&eventHandler);

        Renderer::init(appSpecs.backend);
        Renderer::setClearColor(0.2f, 0.2f, 0.2f);

        fixedTimestep = 1.f / appSpecs.updateRate;
        handleStopOnClose = appSpecs.autoStopOnClose;
    }

    void App::run()
    {
        load();

        window.makeVisible(true);

        ASYNC_UPDATE
        (
            window.makeContextCurrent(false);
            std::thread updateThread{&App::updateRender, this};
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

                updateApp(frameTime);

                renderApp();

                window.swapBuffers();

                Input::mouseWheelScroll = vec2f{};
            )
        }

        ASYNC_UPDATE
        (
            if(updateThread.joinable())
                updateThread.join();
        )

        unload();
    }

    void App::stop()
    {
        running = false;
        window.postEmptyEvent();
    }

    void App::updateApp(float frameTime)
    {
        static float frameTimeAccumulator{};
        frameTimeAccumulator += frameTime;

        while(frameTimeAccumulator >= fixedTimestep)
        {
            fixedUpdate(fixedTimestep);
            frameTimeAccumulator -= fixedTimestep;
        }

        update(frameTime);
    }

    void App::renderApp()
    {
        Renderer::clearBackBuffers();

        Renderer::beginDrawing();

            draw();

        Renderer::endDrawing();
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
        
        void App::updateRender()
        {
            window.makeContextCurrent(true);
            Timer timer;

            while(running)
            {
                float frameTime = timer.elapsed();

                eventHandler.process();

                updateApp(frameTime);

                if(resizing)
                {
                    synchronizeResize();

                    // Because of the way we do multithreading, ImGui widgets start to flicker on resize when VSync is disabled
                    // Waiting here for atleast 4ms seems to eliminate the flickering for the most part (have to test elsewhere)

                    // Also, while testing the framework with lower CPU clock speeds (notebook unplugged), there's some flickering
                    // when running on release even with VSync enabled. So it's better to slowdown a bit, regardless of configuration

                    //if(!OSWindow::isVSynced())
                        wait(4_ms);

                    continue;
                }

                renderApp();

                Input::mouseWheelScroll = vec2f{};

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
            renderApp();

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

    void App::onAppEvent(Event& event)
    {
        if(Event::is_a<WindowClosed>(event) && handleStopOnClose)
            stop();

        Event::dispatch<MouseWheelScrolled, &updateMouseScrollInput>(event);

        onEvent(event);
    }

    App& App::getInstance()
    {
        MIRR_ASSERT(appInstance, "No application instance yet");
        return *appInstance;
    }

    App::~App()
    {
        Renderer::shutdown();
    }
} // namespace mirras
