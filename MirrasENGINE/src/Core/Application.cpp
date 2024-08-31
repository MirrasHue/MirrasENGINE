#include "Core/Application.h"
#include "Core/Timer.h"
#include "Core/Asserts.h"

#include "UI/ImGui.h"
#include "Input/Input.h"

#include <thread>
#include <ranges>

#define GLAD_GL_IMPLEMENTATION // Necessary for using Glad2 header only version
#include <glad/glad.h>

namespace mirras
{
    App::App(const AppSpecs& appSpecs, const WindowSpecs& windowSpecs) :
        window{windowSpecs}
    {
        MIRR_ASSERT_CORE(appInstance == nullptr, "One instance of application already exists");
        appInstance = this;

        Log::initAppLog(appSpecs.name);

        imgui::init();
        
        window.setOnEventCallback([this](Event& event)
        {
            App::onEvent(event);
        });

        fixedTimestep = 1.f / appSpecs.updateRate;
    }

    void App::updateLayers(float frameTime)
    {
        static float frameTimeAccumulator{};
        frameTimeAccumulator += frameTime;

        while(frameTimeAccumulator >= fixedTimestep)
        {
            for(auto& layer : layers)
                layer->update(fixedTimestep);

            frameTimeAccumulator -= fixedTimestep;
        }
    }

    void App::renderLayers()
    {
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        for(auto& layer : layers)
            layer->draw();

        imgui::beginFrame();

            for(auto& layer : layers)
                layer->drawImGui();
                
        imgui::endFrame();
    }

    void App::handleResize()
    {
        window.makeContextCurrent(false);

        switchContext = true;
        switchContext.notify_one();

        resizing.wait(true); // Wait until we are notified that 'resizing' was set to false

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
            }

            if(resizing)
            {
                handleResize();
                continue;
            }

            window.swapBuffers();
        }
    }
    
    void App::run()
    {
        window.makeContextCurrent(false);
        
        updateThread = std::thread{&App::update, this};

        while(running)
        {
            window.waitEvents();

        }
    }

    void App::onWindowResize(WindowResized& event)
    {
        if(event.windowSize.x == 0 || event.windowSize.y == 0)
            return;

        resizing = true;
        
        switchContext.wait(false); // Wait until we are notified that 'switchContext' was set to true

        window.makeContextCurrent(true);

        auto [width, height] = window.getFramebufferSize();
        glViewport(0, 0, width, height);

        // No need to render here anymore, still have to swap the buffers to keep rendering on resize

        window.swapBuffers();
        
        window.makeContextCurrent(false);
        
        // Set it back to false, otherwise, 'switchContext.wait(false)' will not wait on subsequent resizes
        switchContext = false;

        resizing = false;
        resizing.notify_one();
    }

    void App::onWindowClose(WindowClosed& event)
    {
        ENGINE_LOG_INFO("Window Close Event");
        running = false;
    }

    void App::onEvent(Event &event)
    {
        Event::dispatch_to_member<WindowResized, App::onWindowResize>(event, this);
        Event::dispatch_to_member<WindowClosed, App::onWindowClose>(event, this);

        imgui::onEvent(event);

        std::lock_guard lock{layersMutex};

        for(auto& layer : layers | std::views::reverse)
        {
            if(!event.propagable)
                break;
            layer->onEvent(event);
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

    void App::addLayer(std::unique_ptr<Layer> layer)
    {
        std::lock_guard lock{layersMutex};
        layers.addLayer(std::move(layer));
    }

    void App::addOverlay(std::unique_ptr<Layer> layer)
    {
        std::lock_guard lock{layersMutex};
        layers.addOverlay(std::move(layer));
    }

    App::~App()
    {
        running = false;
        if(updateThread.joinable())
            updateThread.join();

        imgui::shutdown();
    }
}