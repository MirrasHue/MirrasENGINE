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

    }

    inline void draw(SpriteComponent& sprite)
    {

    }

    inline void draw(RectangleComponent& rectangle)
    {

    }

    inline void draw(CircleComponent& circle)
    {

    }

    inline void draw(TextComponent& text)
    {

    }
} // namespace mirras
