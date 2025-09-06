#pragma once

#include "Scene/Components.h"
#include "Scene/Entity.h"

#include <imgui/imgui.h>

namespace mirras
{
    template<typename CompType>
    inline void drawComponent(Entity entity, const char* compName)
    {
        constexpr auto flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

        if(ImGui::TreeNodeEx(compName, flags))
        {
            auto& component = entity.get<CompType>();
            // Calculate the first column width based on the longest string that will be displayed
            // on it. This takes into acount variable font sizes, which I intend to add later
            draw(component, ImGui::CalcTextSize("Translation").x + 10.f);

            ImGui::TreePop();
        }
    }

    void draw(TransformComponent& transform, float firstColumnWidth);
    void draw(CameraComponent& camera, float firstColumnWidth);
    void draw(SpriteComponent& sprite, float firstColumnWidth);
    void draw(RectangleComponent& rectangle, float firstColumnWidth);
    void draw(CircleComponent& circle, float firstColumnWidth);
    void draw(TextComponent& text, float firstColumnWidth);
} // namespace mirras
