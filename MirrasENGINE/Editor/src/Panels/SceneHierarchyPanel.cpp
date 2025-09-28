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

        static bool entityAdded = false;

        if(entityAdded)
        {
            ImGui::SetNextItemOpen(true);
            entityAdded = false;
        }

        if(ImGui::TreeNodeEx(scene->name.c_str(), flags | ImGuiTreeNodeFlags_DefaultOpen))
        {
            if(ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                context->selectedEntity = {};

            Entity forDeletion;

            // Iterating through all entities in reverse because they are being given in the reverse order of insertion
            for(auto enttID : registry.view<entt::entity>() | std::views::reverse)
            {
                Entity entity{enttID, &registry};
                drawNode(entity, flags, forDeletion);
            }

            if(forDeletion)
            {
                context->scene->deleteEntity(forDeletion);

                if(forDeletion == context->selectedEntity)
                    context->selectedEntity = {};

                registry.sort<entt::entity>(std::greater{});
            }

            ImGui::TreePop();
        }

        if(ImGui::BeginPopupContextWindow("Create", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
        {
            if(ImGui::MenuItem("Create Entity"))
            {
                uint32& entityCount = context->addedEntityCount;
                // Added a hint here so that the IDs of deleted entities are not reused
                // (solves the problem of wrong ordering when displaying the entities after deletion)
                auto entity = scene->createEntityWithHint(entityCount);
                auto& tag = entity.add<TagComponent>("Entity " + std::to_string(entityCount++));
                context->selectedEntity = entity;
                entityAdded = true;
            }

            ImGui::EndPopup();
        }

        // Avoids a registry dangling ptr when the scenes vector resizes
        if(context->selectedEntity)
            context->selectedEntity.registry = &registry;

        ImGui::End();
    }

    void SceneHierarchyPanel::drawNode(Entity entity, int32 treeNodeFlags, Entity& forDeletion)
    {
        auto& name = entity.get<TagComponent>().tag;

        if(context->selectedEntity == entity)
            treeNodeFlags |= ImGuiTreeNodeFlags_Selected;

        ImGui::PushID((uint32)entity);
        bool opened = ImGui::TreeNodeEx(name.c_str(), treeNodeFlags);

        if(ImGui::IsItemClicked())
            context->selectedEntity = entity;

        if(ImGui::BeginPopupContextItem())
        {
            if(ImGui::MenuItem("Delete Entity"))
                forDeletion = entity;

            ImGui::EndPopup();
        }
        ImGui::PopID();

        if(opened)
            ImGui::TreePop();
    }
} // namespace mirras
