#pragma once

#include "Core/Fwd.h"
#include "Utilities/UUID.h"

#include <entt/entity/registry.hpp>

#include <box2d/id.h>

#include <string>

namespace mirras
{
    class Scene
    {
    public:
        Scene() = default;
        Scene(std::string sceneName) :
            name{std::move(sceneName)} {}

        Entity createEntity(UUID uuid = {});
        Entity createEntityWithHint(uint32 hint, UUID uuid = {});
        Entity createEntity(std::string tag);
        void deleteEntity(Entity entity);

        void update(float dt);
        void fixedUpdate(float ts);
        void draw();
        void draw(const Camera2D& camera);
        void onEvent(Event& event);

        void onRuntimeStart();
        void onRuntimeStop();

    public:
        std::string name;
        entt::registry registry;

    private:
        b2WorldId physicsWorldId = {};
        //b2WorldDef physicsDef = b2DefaultWorldDef();
    };

} // namespace mirras

