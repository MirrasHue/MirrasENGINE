#pragma once

#include "Scene/Entity.h"
#include "Scene/Components.h"

#include "Widgets/Entity.h"

#include <entt/meta/factory.hpp>

namespace mirras::reflect
{
    template<typename... Args>
    inline entt::meta_any call(const char* func_name, entt::id_type type_id, Args&&... args)
    {
        auto meta_type = entt::resolve(type_id);

        if(!meta_type)
        {
            ENGINE_LOG_ERROR("No meta_type for the given type_id, verify if the hash assigned in the meta_factory matches the one being provided");
            return {};
        }

        if(auto func = meta_type.func(entt::hashed_string{func_name}); func)
            return func.invoke({}, std::forward<Args>(args)...);

        ENGINE_LOG_ERROR("A function with the name [{}] wasn't registered or it is misspelled", func_name);
        return {};
    }

    template<typename CompType>
    inline void registerFunctions(const char* name)
    {
        entt::meta_factory<CompType>{}
            .type(entt::type_hash<CompType>::value(), name)
            .template func<&drawComponent<CompType>>("drawComponent()") // Defined in Widgets/Entity.h
            .template func<&addComponent<CompType>>("addComponent()");
    }

    inline void registerComponentsFunctions()
    {
        registerFunctions<TransformComponent>("Transform");
        registerFunctions<ScriptComponent>("Script");
        registerFunctions<CameraComponent>("Camera");
        registerFunctions<SpriteComponent>("Sprite");
        registerFunctions<RectangleComponent>("Rectangle");
        registerFunctions<CircleComponent>("Circle");
        registerFunctions<TextComponent>("Text");
    }
} // namespace mirras
