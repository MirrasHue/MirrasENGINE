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
            handle{enttHandle}, registry{enttRegistry} {}

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

        bool operator <=>(const Entity& rhs) const = default;

        operator uint32() { return (uint32)handle; }
    
    private:
        entt::entity handle = entt::null;
        entt::registry* registry = nullptr;
    };
    

    class EntityScript
    {
    public:
        virtual void onInit() {}
        virtual void onUpdate(float dt) {}
        virtual void onFixedUpdate(float dt) {}
        virtual void onCleanup() {}

        template<typename T>
        T& getComponent()
        {
            return entity.get<T>();
        }

        virtual ~EntityScript() = default;

    private:
        Entity entity;
        friend class Scene;
    };
} // namespace mirras
