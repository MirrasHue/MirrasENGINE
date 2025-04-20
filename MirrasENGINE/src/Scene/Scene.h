#pragma once

#include "Core/Fwd.h"

#include <string>

#include <entt/entity/registry.hpp>

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
        void draw();
        void onEvent(Event& event);
        
    private:
        entt::registry registry;
        std::string name;
    };

} // namespace mirras

