#include "Panels/SceneHierarchyPanel.h"

#include "Scene/Scene.h"

#include <imgui/imgui.h>

#include <ranges>

namespace mirras
{
    void SceneHierarchyPanel::draw()
    {
        ImGui::Begin("Scene Hierarchy", nullptr, ImGuiWindowFlags_NoCollapse);

        if(!context)
        {
            ImGui::End();
            return;
        }

        // Unselect the entity if we switch scenes
        if(context->id != currentSceneID)
        {
            selectedEntity = {};
            currentSceneID = context->id;
        }

        int32 flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

        if(ImGui::TreeNodeEx(context->name.c_str(), flags | ImGuiTreeNodeFlags_DefaultOpen))
        {
            if(ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
                selectedEntity = {};

            // Iterating through all entities in reverse because they are being given in the reverse order of insertion
            for(auto enttID : context->registry.view<entt::entity>() | std::views::reverse)
            {
                Entity entity{enttID, &context->registry};
                drawNode(entity, flags);
            }

            ImGui::TreePop();
        }

        // Avoids a registry dangling ptr when the scenes vector resizes
        if(selectedEntity)
            selectedEntity.registry = &context->registry;

        ImGui::End();
    }

    void SceneHierarchyPanel::drawNode(Entity entity, int32 treeNodeFlags)
    {
        auto& name = entity.get<TagComponent>().tag;

        if(selectedEntity == entity)
            treeNodeFlags |= ImGuiTreeNodeFlags_Selected;

        bool opened = ImGui::TreeNodeEx((void*)(intptr_t)entity.handle, treeNodeFlags, name.c_str());

        if(ImGui::IsItemClicked())
            selectedEntity = entity;

        if(opened)
            ImGui::TreePop();
    }
} // namespace mirras
