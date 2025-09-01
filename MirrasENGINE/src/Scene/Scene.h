#pragma once

#include "Core/Fwd.h"
#include "Utilities/UUID.h"

#include <entt/entity/registry.hpp>

#include <string>

namespace mirras
{
    class Scene
    {
    public:
        Scene() = default;
        Scene(std::string sceneName) :
            name{std::move(sceneName)} {}

        Entity createEntity();
        Entity createEntity(std::string tag);

        void update(float dt);
        void draw(Camera2D& camera);
        void draw();
        void onEvent(Event& event);
        
    public:
        std::string name;

    private:
        entt::registry registry;
        UUID id;

        friend class EditorLayer;
        friend class SceneHierarchyPanel;
    };

} // namespace mirras

