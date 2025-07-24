#pragma once

#include "Scene/Components.h"

#include <imgui/imgui.h>

namespace mirras
{
    template<typename CompType>
    inline void drawComponent(Entity entity)
    {
        auto& component = entity.get<CompType>();
        draw(component);
    }

    inline void draw(TransformComponent& transform)
    {
        ImGui::Text("Transform");
        ImGui::DragFloat("X", &transform.position.x);
        ImGui::DragFloat("Y", &transform.position.y);
        ImGui::DragFloat("Z", &transform.position.z);
    }

    inline void draw(CameraComponent& camera)
    {
        ImGui::Text("Camera");
    }

    inline void draw(SpriteComponent& sprite)
    {
        ImGui::Text("Sprite");
    }

    inline void draw(RectangleComponent& rectangle)
    {
        ImGui::Text("Rect");
    }

    inline void draw(CircleComponent& circle)
    {
        ImGui::Text("Circle");
    }

    inline void draw(TextComponent& text)
    {
        ImGui::Text("Text");
    }
} // namespace mirras
