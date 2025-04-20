#pragma once

#include "Scene/Components.h"

#include <entt/entity/registry.hpp>

namespace mirras
{
    class Entity
    {
    public:
        Entity() = default;
        Entity(entt::entity enttHandle, entt::registry* enttRegistry) :
            handle(enttHandle), registry(enttRegistry) {}

        // Adds a component and returns a reference to it
        template<typename T, typename... Args>
        T& add(Args&&... args)
        {
            return registry->emplace<T>(handle, std::forward<Args>(args)...);
        }

        template<typename ScriptType>
        CppScriptComponent& addCppScript()
        {
            auto& script = registry->emplace<CppScriptComponent>(handle);
            script.instantiateScript = [](){ return static_cast<single_ref<EntityScript>>(instantiate<ScriptType>()); };
            
            return script;
        }

        template<typename T>
        T& get()
        {
            return registry->get<T>(handle);
        }

        template<typename T>
        auto remove()
        {
            return registry->remove<T>(handle);
        }
    
    private:
        entt::entity handle = entt::null;
        entt::registry* registry;
    };
    

    class EntityScript
    {
    public:
        virtual ~EntityScript() = default;

        virtual void onInit() {};
        virtual void onUpdate(float dt) {};
        virtual void onCleanup() {};

        template<typename T>
        T& getComponent()
        {
            return entity.get<T>();
        }

    private:
        Entity entity;
        friend class Scene;
    };
} // namespace mirras
