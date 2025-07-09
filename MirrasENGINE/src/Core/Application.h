#pragma once

#include "Core/OSWindow.h"
#include "Core/AppLayers.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Fwd.h"

#include <string_view>

#ifdef RUN_UPDATE_THREAD
    #include <thread>
    #include <mutex>
    #include <atomic>

    #define ASYNC_UPDATE(...) __VA_ARGS__
    #define NO_ASYNC_UPDATE(...) // For when we need branching
#else
    #define ASYNC_UPDATE(...)
    #define NO_ASYNC_UPDATE(...) __VA_ARGS__
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
    };

    class App
    {
    public:
        App(const AppSpecs& AppSpecs, const WindowSpecs& windowSpecs);

        void run();

        static App& getInstance();
        static OSWindow& getOSWindow() { return App::getInstance().window; }
        
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
        void onWindowClose(WindowClosed& event);

    private:
        OSWindow window;
        AppLayers layers;

        inline static App* appInstance = nullptr;

        ASYNC_UPDATE
        (
            std::mutex layersMutex;
            std::thread updateThread;
            std::atomic_bool switchContext = false;
            std::atomic_bool resizing = false;
            std::atomic_bool running = true;
        )

        NO_ASYNC_UPDATE (
            bool running = true;
        )

        float fixedTimestep = 0.f;
    };
} // namespace mirras
