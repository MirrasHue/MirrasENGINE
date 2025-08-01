#include "EditorLayer.h"

#include "Core/Renderer/Renderer.h"

#include "Events/WindowEvents.h"

#include "Reflection.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace mirras
{
    void EditorLayer::load()
    {
        reflect::registerComponentsUIFunction();

        ImGuiIO& io = ImGui::GetIO();

        io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Fonts/bahnschrift.ttf", fontSize);

        auto scene1 = instantiate<Scene>("Scene1");
        auto scene2 = instantiate<Scene>("Scene2");

        auto cameraEntity = scene1->createEntity("Camera");
        auto& camera = cameraEntity.add<CameraComponent>();

        auto textEntity = scene1->createEntity("Text");
        auto& text = textEntity.add<TextComponent>();
        text.font = instantiate<Font>("Assets/Fonts/consolas.ttf");
        text.text = L"Hello World!";

        auto rectEntity = scene1->createEntity("Square");
        auto& rect = rectEntity.add<RectangleComponent>();
        rect.size  = {200, 200};
        rect.color = {0, 0, 1, 1};

        auto rectEntity2 = scene2->createEntity("Rectangle");
        auto& rect2 = rectEntity2.add<RectangleComponent>();
        rect2.size  = {200, 200};
        rect2.color = {1, 0, 0, 1};

        scenes.emplace_back(std::move(scene1));
        scenes.emplace_back(std::move(scene2));
    }

    void EditorLayer::update(float dt)
    {
        std::erase_if(scenes, [](const auto& scene){ return !scene.open; });

        for(auto& editorScene : scenes)
        {
            editorScene.canvas.resize(editorScene.size.x, editorScene.size.y);

            switch(editorScene.state)
            {
                case SceneState::Editing:
                {
                    if(editorScene.focused)
                        cameraController.update(dt);

                    if(editorScene.hovered)
                        zoomController.updateZoom();

                    break;
                }

                case SceneState::Playing:
                {
                    editorScene.scene->update(dt);
                    break;
                }
            }
        }
    }

    void EditorLayer::draw()
    {
        for(auto& editorScene : scenes)
        {
            Renderer::beginTextureDrawing(editorScene.canvas);
                Renderer::clearBackBuffers();

                switch(editorScene.state)
                {
                    case SceneState::Editing:
                        editorScene.scene->draw(editorScene.camera);
                        break;

                    case SceneState::Playing:
                        editorScene.scene->draw();
                        break;
                }

            Renderer::endTextureDrawing();
        }
    }

    void EditorLayer::drawImGui()
    {
        auto mainDockID = ImGui::DockSpaceOverViewport();

        if(ImGui::BeginMainMenuBar())
        {
            if(ImGui::BeginMenu("File"))
            {

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        //ImGui::SetNextWindowSizeConstraints({800, 450}, {FLT_MAX, FLT_MAX});
        activeScene = nullptr;

        for(auto& editorScene : scenes)
        {
            if(!editorScene.open)
                continue;

            ImGui::SetNextWindowDockID(mainDockID, ImGuiCond_FirstUseEver);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
            ImGui::Begin(editorScene.scene->name.c_str(), &editorScene.open, ImGuiWindowFlags_NoCollapse);
            {
                auto [width, height] = ImGui::GetContentRegionAvail();
                editorScene.size = {width, height};

                if(ImGui::GetCurrentWindow()->Hidden)
                {
                    ImGui::End();
                    ImGui::PopStyleVar();
                    continue;
                }

                editorScene.focused = false;

                if(ImGui::IsWindowFocused())
                {
                    cameraController.setCamera(&editorScene.camera);
                    editorScene.focused = true;
                    activeScene = &editorScene;
                }

                editorScene.hovered = false;

                if(ImGui::IsWindowHovered())
                {
                    zoomController.setCamera(&editorScene.camera);
                    editorScene.hovered = true;
                }

                // So that we keep showing the hierarchy panel for this scene when its tab is selected but not focused
                if(!activeScene && ImGui::GetCurrentWindow()->DockTabIsVisible)
                    activeScene = &editorScene;

                ImGui::Image(editorScene.canvas.color->id, {width, height}, {0, 1}, {1, 0});
            }
            ImGui::End();
            ImGui::PopStyleVar();
        }

        sceneHierarchy.setContext(activeScene);
        sceneHierarchy.draw();

        entityProperties.draw(activeScene ? activeScene->selectedEntity : Entity{});

        ImGui::Begin("Content Browser", nullptr, ImGuiWindowFlags_NoTitleBar);

        ImGui::End();

        ImGui::ShowDemoWindow();
    }

    void EditorLayer::onEvent(Event& event)
    {
        if(Event::is_a<WindowClosed>(event))
            App::getInstance().stop();
    }
} // namespace mirras
