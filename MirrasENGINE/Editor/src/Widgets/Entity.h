#pragma once

#include "Scene/Components.h"
#include "Scene/Entity.h"

#include <imgui/imgui.h>

namespace mirras
{
    template<typename CompType>
    inline void drawComponent(Entity entity, const char* compName)
    {
        constexpr auto flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth |
            ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowOverlap;

        bool opened = ImGui::TreeNodeEx(compName, flags);

        float frameHeight = ImGui::GetFrameHeight();
        float buttonPosX = ImGui::GetContentRegionAvail().x - frameHeight + ImGui::GetStyle().FramePadding.x * 2.f;

        if(opened)
            buttonPosX += ImGui::GetStyle().IndentSpacing;

        ImGui::SameLine(buttonPosX);

        ImGui::PushID(compName);
        ImGui::PushStyleVarX(ImGuiStyleVar_ButtonTextAlign, 0.58f);

        if(ImGui::Button("-", ImVec2{frameHeight, frameHeight}))
            ImGui::OpenPopup("Options");

        ImGui::PopStyleVar();

        bool removeComponent = false;

        if(ImGui::BeginPopup("Options"))
        {
            if(ImGui::MenuItem("Remove Component"))
                removeComponent = true;

            ImGui::EndPopup();
        }
        ImGui::PopID();

        if(opened)
        {
            auto& component = entity.get<CompType>();

            ImGui::PushID((uint32)entity); // So that the editing of one entity doesn't affect another
    
            // Calculate the first column width based on the longest string that will be displayed
            // on it. This takes into acount variable font sizes, which I intend to add later
            draw(component, ImGui::CalcTextSize("Translation").x + 10.f);
    
            ImGui::PopID();

            ImGui::TreePop();
        }

        if(removeComponent)
            entity.remove<CompType>();
    }

    template<typename CompType>
    inline void addComponent(Entity entity)
    {
        entity.add<CompType>();
    }

    void draw(TransformComponent& transform, float firstColumnWidth);
    void draw(CameraComponent& camera, float firstColumnWidth);
    void draw(SpriteComponent& sprite, float firstColumnWidth);
    void draw(RectangleComponent& rectangle, float firstColumnWidth);
    void draw(CircleComponent& circle, float firstColumnWidth);
    void draw(TextComponent& text, float firstColumnWidth);
} // namespace mirras
