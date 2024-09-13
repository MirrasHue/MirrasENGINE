#include "EditorLayer.h"

#include "Events/KeyboardEvents.h"

#include <imgui/imgui.h>

namespace mirras
{
    void EditorLayer::drawImGui()
    {
        ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
        ImGui::ShowDemoWindow();
    }

    void EditorLayer::onEvent(Event& event)
    {
        camera.onWindowResize(event);
    }

    void EditorLayer::draw()
    {
        // Temp, later moving to Playground
        Renderer::beginMode2D(camera);
            Renderer::drawTriangle();
        Renderer::endMode2D();
    }
}

