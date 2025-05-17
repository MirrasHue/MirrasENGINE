#include "EditorLayer.h"

#include "Core/Renderer/Renderer.h"

#include "UI/ImGui.h"

#include <imgui/imgui.h>

namespace mirras
{
    void EditorLayer::load()
    {
        ImGuiIO& io = ImGui::GetIO();

        io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Fonts/consolas.ttf", fontSize);
    }

    void EditorLayer::update(float dt)
    {
        canvas.resize(viewportSize.x, viewportSize.y);

        if(viewportFocused)
            cameraController.update(dt);

        imgui::blockEvents(viewportHovered);
    }

    void EditorLayer::draw()
    {
        Renderer::beginTextureDrawing(canvas);
            Renderer::clearBackBuffers();

            Renderer::beginMode2D(editorCamera);
                Renderer::drawCircle({600, 400, 1}, 50, {1,1,1,1});
                Renderer::drawRectangle({0, 0, 1}, {200, 200}, {100, 100}, {0,0,1,1});
                Renderer::drawTriangle({0, 400, 0}, {500, 400, 1}, {250, 0, 1}, {0,1,0,1});
                Renderer::drawShaderCircle({0, 0, 1}, 100, {1,0,0.5,1});
            Renderer::endMode2D();

        Renderer::endTextureDrawing();
    }

    void EditorLayer::drawImGui()
    {
        ImGui::DockSpaceOverViewport();

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


        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoTitleBar);
        {
            auto [width, height] = ImGui::GetContentRegionAvail();
            viewportSize = {width, height};

            viewportFocused = ImGui::IsWindowFocused();
            viewportHovered = ImGui::IsWindowHovered();

            ImGui::Image(canvas.color->id, {viewportSize.x, viewportSize.y}, {0,1}, {1,0});
        }
        ImGui::End();
        ImGui::PopStyleVar();


        ImGui::Begin("Content Browser", nullptr, ImGuiWindowFlags_NoTitleBar);

        ImGui::End();
        
        ImGui::ShowDemoWindow();
    }

    void EditorLayer::onEvent(Event& event)
    {
        cameraController.onMouseWheelScroll(event);
    }
} // namespace mirras
