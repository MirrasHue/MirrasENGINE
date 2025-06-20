#pragma once

#include "Core/Fwd.h"

#include <entt/entity/registry.hpp>

#include <string>

namespace mirras
{
    class Scene
    {
    public:
        Scene() = default;
        Scene(std::string sceneName) :
            name(sceneName) {}

        Entity createEntity();

        void update(float dt);
        void draw(Camera2D& camera);
        void draw();
        void onEvent(Event& event);
        
    public:
        std::string name;

    private:
        entt::registry registry;
    };

} // namespace mirras

