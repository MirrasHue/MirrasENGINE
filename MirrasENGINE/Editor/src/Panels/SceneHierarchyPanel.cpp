#include "Panels/SceneHierarchyPanel.h"

#include "EditorScene.h"

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

        auto& scene = context->scene;
        auto& registry = scene->registry;
        auto flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

        if(ImGui::TreeNodeEx(scene->name.c_str(), flags | ImGuiTreeNodeFlags_DefaultOpen))
        {
            if(ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
                context->selectedEntity = {};

            // Iterating through all entities in reverse because they are being given in the reverse order of insertion
            for(auto enttID : registry.view<entt::entity>() | std::views::reverse)
            {
                Entity entity{enttID, &registry};
                drawNode(entity, flags);
            }

            ImGui::TreePop();
        }

        // Avoids a registry dangling ptr when the scenes vector resizes
        if(context->selectedEntity)
            context->selectedEntity.registry = &registry;

        ImGui::End();
    }

    void SceneHierarchyPanel::drawNode(Entity entity, int32 treeNodeFlags)
    {
        auto& name = entity.get<TagComponent>().tag;

        if(context->selectedEntity == entity)
            treeNodeFlags |= ImGuiTreeNodeFlags_Selected;

        bool opened = ImGui::TreeNodeEx((void*)(intptr_t)entity.handle, treeNodeFlags, name.c_str());

        if(ImGui::IsItemClicked())
            context->selectedEntity = entity;

        if(opened)
            ImGui::TreePop();
    }
} // namespace mirras
