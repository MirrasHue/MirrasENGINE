#include "EditorLayer.h"

#include "Core/Renderer/Renderer.h"

#include "Events/KeyboardEvents.h"

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

    }

    void EditorLayer::draw()
    {
        // We have to call resize here instead of in update, because if the imgui viewport resizes, this has
        // to reflect the change right after, and update might not be called every frame (fixed update rate)
        canvas.resize(viewportSize.x, viewportSize.y);
        
        Renderer::beginTextureDrawing(canvas);
            Renderer::clearBackBuffers();
            
            Renderer::drawCircle({600, 400, 1}, 50, {1,1,1,1});
            Renderer::drawRectangle({0, 0, 1}, {200, 200}, {100, 100}, {0,0,1,1});
            Renderer::drawTriangle({0, 400, 0}, {500, 400, 1}, {250, 0, 1}, {0,1,0,1});
            Renderer::drawShaderCircle({0, 0, 1}, 100, {1,0,0.5,1});

        Renderer::endTextureDrawing();
    }

    void EditorLayer::drawImGui()
    {
        ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

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
            ImVec2 pos = ImGui::GetCursorScreenPos();

            ImGui::GetWindowDrawList()->AddImage(canvas.color->id, pos, {pos.x + viewportSize.x, pos.y + viewportSize.y}, {0,1}, {1,0});
            
            auto [width, height] = ImGui::GetContentRegionAvail();
            viewportSize = {width, height};
        }
        ImGui::End();
        ImGui::PopStyleVar();


        ImGui::Begin("Content Browser", nullptr, ImGuiWindowFlags_NoTitleBar);

        ImGui::End();
        
        ImGui::ShowDemoWindow();
    }

    void EditorLayer::onEvent(Event& event)
    {

    }
} // namespace mirras

