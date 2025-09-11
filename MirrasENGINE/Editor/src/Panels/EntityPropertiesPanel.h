#pragma once

#include "Scene/Entity.h"

#include "Reflection.h"

#include <imgui/imgui.h>

#include <string.h>

namespace mirras
{
    // Unfortunately ImGui doesn't clamp pasted text that exceeds the buffer size (it is completely
    // ignored, instead of pasting what could still fit). It seems to be a limitation of stb_textedit
    template<unsigned N>
    bool InputText(const char* label, std::string& source, ImGuiInputTextFlags flags = 0)
    {
        char buffer[N] = {};
        strncpy_s(buffer, sizeof(buffer), source.c_str(), N - 1);

        ImGui::PushID(&source);

        bool modified = ImGui::InputText(label, buffer, sizeof(buffer), flags);

        ImGui::PopID();

        if(modified)
            source = std::string{buffer};

        return modified;
    }

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

            bool addComponent = false;

            if(ImGui::BeginTable("Tag + AddComponent", 2))
            {
                float paddingX = ImGui::GetStyle().FramePadding.x * 2.f;

                ImGui::TableSetupColumn("");
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("Add Component").x + paddingX);

                ImGui::TableNextColumn();
                {
                    auto& name = entity.get<TagComponent>().tag;
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    InputText<36>("##tag", name);
                }

                ImGui::TableNextColumn();
                {
                    if(ImGui::Button("Add Component"))
                        addComponent = true;
                }

                ImGui::EndTable();
            }

            struct ComponentInfo
            {
                const char* name = nullptr;
                entt::id_type type_id = entt::null;
            };

            std::vector<ComponentInfo> componentsToAdd;

            // Iterate through all the reflected/registered components
            for(const auto [type_id, meta] : entt::resolve())
            {
                auto* storage = entity.registry->storage(type_id);

                if(storage && storage->contains(entity))
                    reflect::call("drawComponent()", type_id, entity, meta.name());
                else
                    componentsToAdd.emplace_back(ComponentInfo{meta.name(), type_id});
            }

            if(addComponent)
                ImGui::OpenPopup("AddComponent");

            if(ImGui::BeginPopup("AddComponent"))
            {
                for(auto [compName, type_id] : componentsToAdd)
                    if(ImGui::MenuItem(compName))
                        reflect::call("addComponent()", type_id, entity);

                if(componentsToAdd.empty())
                    ImGui::MenuItem("No more components to add");

                ImGui::EndPopup();
            }

            ImGui::End();
        }
    };
} // namespace mirras
