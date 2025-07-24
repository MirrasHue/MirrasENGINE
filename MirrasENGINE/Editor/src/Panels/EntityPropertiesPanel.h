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

            for(const auto [type_id, meta] : entt::resolve())
            {
                auto* storage = entity.registry->storage(type_id);

                if(storage && storage->contains(entity))
                    reflect::call("drawComponent", type_id, entity);
            }

            ImGui::End();
        }
    };
} // namespace mirras
