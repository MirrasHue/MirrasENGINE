#pragma once

#include <memory>
#include <string_view>
#include <mutex>
#include <atomic>

#include "Core/OSWindow.h"
#include "Core/AppLayers.h"
#include "Core/Log.h"

#include "Events/WindowEvents.h"
#include "Events/KeyboardEvents.h"
#include "Events/MouseEvents.h"

#include "Core/Renderer/Renderer.h"

namespace mirras
{
    // Defined by the client
    inline std::unique_ptr<class App> createClientApp();

    struct AppSpecs
    {
        std::string_view name = "MirrasENGINE";
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
        
        void addLayer(std::unique_ptr<Layer> layer);
        void addOverlay(std::unique_ptr<Layer> layer);

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
}