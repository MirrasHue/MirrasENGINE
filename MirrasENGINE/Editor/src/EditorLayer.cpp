#include "EditorLayer.h"

#include "Core/Renderer/Renderer.h"

#include "UI/ImGui.h"

#include <imgui/imgui.h>
//#include <imgui/imgui_internal.h>

namespace mirras
{
    void EditorLayer::load()
    {
        ImGuiIO& io = ImGui::GetIO();

        io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Fonts/bahnschrift.ttf", fontSize);

        auto scene1 = instantiate_shared<Scene>("Scene1");
        auto scene2 = instantiate_shared<Scene>("Scene2");

        auto cameraEntity = scene1->createEntity();
        auto& camera = cameraEntity.add<CameraComponent>();

        auto textEntity = scene1->createEntity();
        auto& text = textEntity.add<TextComponent>();
        text.font = instantiate<Font>("Assets/Fonts/consolas.ttf");
        text.text = L"Hello World!";

        auto rectEntity = scene1->createEntity();
        auto& rect = rectEntity.add<RectangleComponent>();
        rect.size  = {200, 200};
        rect.color = {0, 0, 1, 1};

        scenes.emplace_back(scene1);
        scenes.emplace_back(scene2);

        activeScene = scene1;
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
                        camControllerHovered.updateZoom();

                    break;
                }

                case SceneState::Playing:
                {
                    editorScene.scene->update(dt);
                    break;
                }
            }
        }

        //imgui::ignoreEventCapturing(viewportFocused || viewportHovered);
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

        ImGui::Begin("Panel", nullptr, ImGuiWindowFlags_NoTitleBar);

        ImGui::End();

        //ImGui::SetNextWindowSizeConstraints({800, 450}, {FLT_MAX, FLT_MAX});
        for(auto& editorScene : scenes)
        {
            if(!editorScene.open)
                continue;

            ImGui::SetNextWindowDockID(mainDockID, ImGuiCond_FirstUseEver);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
            ImGui::Begin(editorScene.scene->name.c_str(), &editorScene.open, ImGuiWindowFlags_NoCollapse);
            {
                // ImGui::GetCurrentWindow()->Hidden
                editorScene.focused = false;

                if(ImGui::IsWindowFocused())
                {
                    activeScene = editorScene.scene;
                    cameraController.setCamera(&editorScene.camera);
                    editorScene.focused = true;
                }

                editorScene.hovered = false;

                if(ImGui::IsWindowHovered())
                {
                    camControllerHovered.setCamera(&editorScene.camera);
                    editorScene.hovered = true;
                }

                auto [width, height] = ImGui::GetContentRegionAvail();
                editorScene.size = {width, height};

                ImGui::Image(editorScene.canvas.color->id, {width, height}, {0, 1}, {1, 0});
            }
            ImGui::End();
            ImGui::PopStyleVar();
        }

        ImGui::Begin("Content Browser", nullptr, ImGuiWindowFlags_NoTitleBar);

        ImGui::End();

        ImGui::ShowDemoWindow();
    }

    void EditorLayer::onEvent(Event& event)
    {
        
    }
} // namespace mirras
