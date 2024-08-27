#include "EditorLayer.h"

#include "Events/KeyboardEvents.h"

#include <imgui/imgui.h>

namespace mirras
{
    void EditorLayer::drawImGui()
    {
        ImGui::ShowDemoWindow();
    }

    void EditorLayer::onEvent(Event& event)
    {
        
    }
}

