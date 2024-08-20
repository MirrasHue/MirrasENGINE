#include "Core/Application.h"
#include "Core/Timer.h"
#include "Core/Asserts.h"

#include "UI/ImGui.h"

#include <thread>
#include <ranges>

#define GLAD_GL_IMPLEMENTATION // Necessary for using Glad2 header only version
#include <glad/glad.h>

namespace mirras
{
    App::App(AppSpecs& specs) :
        window{specs.title, specs.width, specs.height, specs.VSync, specs.fullScreen}
    {
        MIRR_ASSERT_CORE(appInstance == nullptr, "One instance of application already exists");
        appInstance = this;

        Log::initAppLog(specs.title);

        imgui::init();
        
        window.setOnEventCallback([this](Event& event)
        {
            App::onEvent(event);
        });

        fixedTimestep = 1.f / specs.updateRate;
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
        glClearColor(0.5f, 0.5f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        for(auto& layer : layers)
            layer->draw();

        imgui::beginFrame();
        ImGui::ShowDemoWindow();

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
            updateLayers(frameTime);

            if(resizing)
            {
                handleResize();
                continue;
            }

            renderLayers();

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
        resizing = true;
        
        switchContext.wait(false); // Wait until we are notified that 'switchContext' was set to true

        window.makeContextCurrent(true);

        auto [width, height] = window.getFramebufferSize();
        glViewport(0, 0, width, height);
        
        renderLayers();

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

        for(auto& layer : layers | std::views::reverse)
            layer->onEvent(event);

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

    App::~App()
    {
        running = false;
        if(updateThread.joinable())
            updateThread.join();

        imgui::shutdown();
    }
}