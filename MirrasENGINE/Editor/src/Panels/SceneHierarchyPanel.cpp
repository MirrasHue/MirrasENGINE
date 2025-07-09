#include "Panels/SceneHierarchyPanel.h"

#include "Scene/Scene.h"

#include <imgui/imgui.h>

#include <ranges>

namespace mirras
{
    void SceneHierarchyPanel::drawPanel(Scene* scene)
    {
        ImGui::Begin("Scene Hierarchy");

        if(!scene)
        {
            ImGui::End();
            return;
        }

        int32 flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

        if(ImGui::TreeNodeEx(scene->name.c_str(), flags | ImGuiTreeNodeFlags_DefaultOpen))
        {
            if(ImGui::IsItemToggledOpen() || (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left)))
                selectedEntity = {};

            // Iterating through all entities in reverse because they are being given in the reverse order of insertion
            for(auto enttID : scene->registry.view<entt::entity>() | std::views::reverse)
            {
                Entity entity{enttID, &scene->registry};
                drawNode(entity, flags);
            }

            ImGui::TreePop();
        }

        ImGui::End();
    }

    void SceneHierarchyPanel::drawNode(Entity entity, int32 treeNodeFlags)
    {
        auto& name = entity.get<TagComponent>().tag;

        if(selectedEntity == entity)
            treeNodeFlags |= ImGuiTreeNodeFlags_Selected;

        bool opened = ImGui::TreeNodeEx((void*)(intptr_t)entity, treeNodeFlags, name.c_str());

        if(ImGui::IsItemClicked())
            selectedEntity = entity;

        if(opened)
            ImGui::TreePop();
    }
} // namespace mirras
