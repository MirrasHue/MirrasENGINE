#pragma once

#include "Scene/Entity.h"

#include "Reflection.h"

#include <imgui/imgui.h>

namespace mirras
{
    class EntityPropertiesPanel
    {
    public:
        void draw(Entity entity)
        {
            ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoCollapse);

            if(!entity)
            {
                ImGui::End();
                return;
            }

            for(const auto [component_id, storage] : entity.registry->storage())
            {
                if(component_id == entt::type_hash<IDComponent> ::value() ||
                   component_id == entt::type_hash<TagComponent>::value() ||
                   !storage.contains(entity)) continue;

                reflect::call("drawComponent", component_id, entity);
            }

            ImGui::End();
        }
    };
} // namespace mirras
