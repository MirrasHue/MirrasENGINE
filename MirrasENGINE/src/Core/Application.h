#pragma once

#include "Core/OSWindow.h"
#include "Core/AppLayers.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Fwd.h"

#include <thread>
#include <string_view>
#include <mutex>
#include <atomic>

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
        void update();

        static App& getInstance();
        static OSWindow& getOSWindow() { return App::getInstance().window; }
        
        void addLayer(single_ref<Layer> layer);
        void addOverlay(single_ref<Layer> layer);

        ~App();

    private:
        void updateLayers(float frameTime);
        void renderLayers();
        void synchronizeResize();

        void onEvent(Event& event);
        void onWindowResize(WindowResized& event);
        void onWindowClose(WindowClosed& event);

    private:
        OSWindow window;
        std::mutex layersMutex;
        AppLayers layers;
        std::thread updateThread;

        inline static App* appInstance = nullptr;

        float fixedTimestep{};

        std::atomic_bool resizing = false;
        std::atomic_bool switchContext = false;
        std::atomic_bool running = true;
    };
} // namespace mirras
