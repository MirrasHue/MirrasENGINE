#pragma once

#include "Core/OSWindow.h"
#include "Core/Renderer/Renderer.h"
#include "Core/EventHandler.h"
#include "Core/AsyncMacros.h"
#include "Core/Types/Reference.h"
#include "Core/Fwd.h"

#include <string_view>

#ifdef RUN_UPDATE_THREAD
    #include <atomic>
#endif

namespace mirras
{
    struct AppSpecs
    {
        std::string_view workingDirectory;
        Renderer::Backend backend = Renderer::Backend::OpenGL;
        uint16 fixedUpdateRate = 60;
        // Set to false if you need to do extra work before exiting, then call stop()
        bool autoStopOnClose = true;
    };

    class App
    {
    public:
        App(const AppSpecs& AppSpecs, const WindowSpecs& windowSpecs);

        void run();
        void stop();

        static App& getInstance();
        static OSWindow& getOSWindow() { return App::getInstance().window; }

        virtual ~App();

    protected:
        // User defined functions
        virtual void load() {}
        virtual void update(float dt) {}
        virtual void fixedUpdate(float dt) {}
        virtual void onEvent(Event& event) {}
        virtual void draw() {}
        virtual void unload() {}

    private:
        void updateApp(float frameTime);
        void renderApp();

        ASYNC_UPDATE
        (
            void updateRender();
            void synchronizeResize();
            void handleResize(int32 width, int32 height);
        )

        void onAppEvent(Event& event);
        void onWindowResize(WindowResized& event);

    private:
        OSWindow window;
        EventHandler eventHandler;

        inline static App* appInstance = nullptr;

        ASYNC_UPDATE
        (
            std::atomic_bool switchContext = false;
            std::atomic_bool resizing = false;
            std::atomic_bool running = false;
        )

        bool handleStopOnClose = true;

        NO_ASYNC_UPDATE (
            bool running = false;
        )

        float fixedTimestep = 0.f;

        friend class EventHandler;
    };
} // namespace mirras
