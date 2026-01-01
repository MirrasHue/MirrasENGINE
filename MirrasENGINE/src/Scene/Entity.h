#pragma once

#include "Scene/Components.h"
#include "Core/Asserts.h"

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
        T& get() const
        {
            // We could also make this return pointers instead of references, but it would require checking for null every time
            MIRR_ASSERT_CORE(has<T>(), "This entity doesn't have the requested component: {}", entt::type_name<T>::value());

            return registry->get<T>(handle);
        }

        template<typename T>
        T* tryGet() const
        {
            return registry->try_get<T>(handle);
        }

        template<typename... T>
        bool has() const
        {
            return registry->all_of<T...>(handle);
        }

        template<typename T>
        auto remove()
        {
            return registry->remove<T>(handle);
        }

        auto operator <=>(const Entity& rhs) const = default;

        operator bool() { return handle != entt::null; }
        operator entt::entity() { return handle; }
        operator uint32() { return (uint32)handle; }
    
    public:
        entt::entity handle = entt::null;
        entt::registry* registry = nullptr;
    };
    

    class EntityScript
    {
    public:
        virtual void onInit() {}
        virtual void onUpdate(float dt) {}
        virtual void onFixedUpdate(float ts) {}
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
