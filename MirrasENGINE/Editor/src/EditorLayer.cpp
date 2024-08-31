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
        
    }
}

