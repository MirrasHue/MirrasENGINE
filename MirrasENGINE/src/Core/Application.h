#pragma once

#include <memory>
#include <string_view>
#include <atomic>

#include "Core/OSWindow.h"
#include "Core/AppLayers.h"
#include "Core/Log.h"

#include "Events/WindowEvents.h"
#include "Events/KeyboardEvents.h"
#include "Events/MouseEvents.h"

namespace mirras
{
    struct AppSpecs
    {
        std::string_view title = "MirrasENGINE";
        int32 width = 1280;
        int32 height = 720;
        uint16 updateRate = 60;
        bool fullScreen = false;
        bool VSync = true;
    };

    class App
    {
    public:
        App(AppSpecs& specs);
        void run();
        void update();

        static App& getInstance();
        OSWindow&   getOSWindow() { return window; }
        
        void onEvent(Event& event);
        void onWindowResize(WindowResized& event);
        void onWindowClose(WindowClosed& event);

        void addLayer(std::unique_ptr<Layer> layer)   { layers.addLayer(std::move(layer)); }
        void addOverlay(std::unique_ptr<Layer> layer) { layers.addOverlay(std::move(layer)); }

        ~App();

    private:
        void updateLayers(float frameTime);
        void renderLayers();
        void handleResize();

    private:
        OSWindow window;
        AppLayers layers;
        std::thread updateThread;

        inline static App* appInstance{};

        float fixedTimestep{};

        std::atomic_bool resizing = false;
        std::atomic_bool switchContext = false;
        std::atomic_bool running = true;
    };

    // Defined by the client
    inline std::unique_ptr<App> createClientApp();
}