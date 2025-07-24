#pragma once

#include "Core/OSWindow.h"
#include "Core/AppLayers.h"
#include "Core/Renderer/Renderer.h"
#include "Core/EventHandler.h"
#include "Core/AsyncMacros.h"
#include "Core/Fwd.h"

#include <string_view>

#ifdef RUN_UPDATE_THREAD
    #include <atomic>
#endif

namespace mirras
{
    // Defined by the client
    inline single_ref<class App> createClientApp();

    struct AppSpecs
    {
        std::string_view name = "MirrasENGINE";
        std::string_view workingDirectory;
        Renderer::Backend backend = Renderer::Backend::OpenGL;
        uint16 updateRate = 60;
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

        // These should only be called from the ctor of an App derived class, createClientApp() or from load()
        // inside a layer. If they are needed elsewhere (like from a layer update()), we should enqueue them
        // as an event to be executed later, avoiding insertion problems while iterating over the app layers
        void addLayer(single_ref<Layer> layer);
        void addOverlay(single_ref<Layer> layer);

        ~App();

    private:
        void updateLayers(float frameTime);
        void renderLayers();

        ASYNC_UPDATE
        (
            void update();
            void synchronizeResize();
            void handleResize(int32 width, int32 height);
        )

        void onEvent(Event& event);
        void onWindowResize(WindowResized& event);

    private:
        OSWindow window;
        AppLayers layers;
        EventHandler eventHandler;

        inline static App* appInstance = nullptr;

        ASYNC_UPDATE
        (
            std::atomic_bool switchContext = false;
            std::atomic_bool resizing = false;
            std::atomic_bool running = true;
        )

        bool handleStopOnClose = true;

        NO_ASYNC_UPDATE (
            bool running = true;
        )

        float fixedTimestep = 0.f;

        friend class EventHandler;
    };
} // namespace mirras
