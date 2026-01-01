#include "Scene/Scene.h"

#include "Core/Renderer/Renderer.h"
#include "Core/Log.h"

#include "Scene/Entity.h"
#include "Scene/Components.h"

#include <box2d/box2d.h>

#include <sol/table.hpp>

namespace mirras
{
    Entity Scene::createEntity(UUID uuid)
    {
        Entity entity{registry.create(), &registry};
        entity.add<IDComponent>(uuid);
        entity.add<TransformComponent>();

        return entity;
    }

    Entity Scene::createEntityWithHint(uint32 hint, UUID uuid)
    {
        Entity entity{registry.create(entt::entity{hint}), &registry};
        entity.add<IDComponent>(uuid);
        entity.add<TransformComponent>();

        return entity;
    }

    Entity Scene::createEntity(std::string tag)
    {
        auto entity = createEntity();

        auto& name = entity.add<TagComponent>();
        name.tag = tag.empty() ? "Default Entity" : std::move(tag);

        return entity;
    }

    void Scene::deleteEntity(Entity entity)
    {
        if(registry.valid(entity))
            registry.destroy(entity);
    }

    void Scene::update(float dt)
    {
        registry.view<CppScriptComponent>().each([dt](auto enttID, auto& cpp)
        {            
            cpp.script->onUpdate(dt);
        });

        registry.view<ScriptComponent>().each([dt](auto enttID, auto& script)
        {
            if(!script.instance.hasData())
                return;

            sol::table data = script.instance.env[".data"];
            sol::protected_function fn = data["on_update"];

            if(!fn.valid())
                return;

            sol::set_environment(script.instance.env, fn);
            auto result = fn(data, dt);

            if(!result.valid())
            {
                sol::error error = result;
                ENGINE_LOG_ERROR("Error while executing Lua script [on_update()]: {}", error.what());
            }
        });
    }

    void Scene::fixedUpdate(float ts)
    {
        const int32 subSteps = 4;
        b2World_Step(physicsWorldId, ts, subSteps);

        //b2BodyEvents events = b2World_GetBodyEvents(physicsWorldId);
        registry.view<TransformComponent, RigidBodyComponent>().each([this](auto enttID, auto& transform, auto& rigidBody)
        {
            auto bodyTransf = b2Body_GetTransform(rigidBody.id);
            transform.position.x = bodyTransf.p.x;
            transform.position.y = bodyTransf.p.y;
            transform.rotation = glm::degrees(b2Rot_GetAngle(bodyTransf.q));
        });

        registry.view<ScriptComponent>().each([ts](auto enttID, auto& script)
        {
            if(!script.instance.hasData())
                return;

            sol::table data = script.instance.env[".data"];
            sol::protected_function fn = data["on_fixed_update"];

            if(!fn.valid())
                return;

            sol::set_environment(script.instance.env, fn);
            auto result = fn(data, ts);

            if(!result.valid())
            {
                sol::error error = result;
                ENGINE_LOG_ERROR("Error while executing Lua script [on_fixed_update()]: {}", error.what());
            }
        });
    }

    void Scene::draw()
    {
        Camera2D* camera = nullptr;

        registry.view<TransformComponent, CameraComponent>().each([&camera](auto enttID, auto& transform, auto& cameraComp)
        {
            cameraComp.camera.position = glm::vec2{transform.position};
            cameraComp.camera.rotation = transform.rotation;
            camera = &cameraComp.camera;
        });

        if(camera)
        {
            draw(*camera);
            return;
        }

        ENGINE_LOG_WARN("Need a camera with transform component in order to draw the scene");
    }

    void Scene::draw(const Camera2D& camera)
    {
        Renderer::beginMode2D(camera);

        // Sprites
        registry.view<TransformComponent, SpriteComponent>().each([](auto enttID, auto& transform, auto& sprite)
        {
            if(!sprite.texture)
            {
                //ENGINE_LOG_ERROR("SpriteComponent has no texture assigned to it");
                return;
            }

            Renderer::setPixelOutputData((int32)enttID);

            Renderer::drawTexture(*sprite.texture, sprite.texSampleArea, transform.position, transform.scale *
                                  glm::vec2{sprite.texture->width, sprite.texture->height}, {0.f, 0.f}, transform.rotation, sprite.tintColor);
        });

        // Rectangles
        registry.view<TransformComponent, RectangleComponent>().each([](auto enttID, auto& transform, auto& rectangle)
        {
            Renderer::setPixelOutputData((int32)enttID);

            Renderer::drawRectangle(transform.position, rectangle.size * transform.scale, {0.f, 0.f}, rectangle.color, transform.rotation);
        });

        // Circles
        registry.view<TransformComponent, CircleComponent>().each([](auto enttID, auto& transform, auto& circle)
        {
            Renderer::setPixelOutputData((int32)enttID);

            Renderer::drawShaderCircle(transform.position, circle.radius, circle.color, circle.fillFactor, circle.fadeFactor);
        });

        // Text
        registry.view<TransformComponent, TextComponent>().each([](auto enttID, auto& transform, auto& text)
        {
            if(!text.font.atlasTexture)
            {
                //ENGINE_LOG_ERROR("TextComponent has no atlas texture assigned to font");
                return;
            }

            Renderer::setPixelOutputData((int32)enttID);

            Renderer::drawText(text.text, text.font, transform.position, text.fontSize, text.color, text.kerning, text.lineSpacing);
        });

        Renderer::endMode2D();
    }

    void Scene::onEvent(Event& event)
    {
        
    }

    void Scene::onRuntimeStart()
    {
        registry.view<CppScriptComponent>().each([this](auto enttID, auto& cpp)
        {
            if(!cpp.script)
            {
                cpp.script = cpp.instantiateScript();
                cpp.script->entity = Entity{enttID, &registry};
                cpp.script->onInit();
            }
        });

        b2WorldDef def = b2DefaultWorldDef();

        physicsWorldId = b2CreateWorld(&def);

        registry.view<TransformComponent, RigidBodyComponent>().each([this](auto enttID, auto& transform, auto& rigidBody)
        {
            Entity entity{enttID, &registry};

            rigidBody.def.position.x = transform.position.x;
            rigidBody.def.position.y = transform.position.y;
            rigidBody.def.rotation = b2MakeRot(glm::radians(transform.rotation));

            rigidBody.id = b2CreateBody(physicsWorldId, &rigidBody.def);

            auto boxCollider = entity.tryGet<BoxColliderComponent>();

            if(boxCollider)
            {
                b2Polygon box = b2MakeBox(boxCollider->size.x * 0.5f, boxCollider->size.y * 0.5f);
                box.centroid = {boxCollider->offset.x, boxCollider->offset.y};

                boxCollider->id = b2CreatePolygonShape(rigidBody.id, &boxCollider->def, &box);
            }

            auto circleCollider = entity.tryGet<CircleColliderComponent>();

            if(circleCollider)
            {
                b2Circle circle;
                circle.center = {circleCollider->offset.x, circleCollider->offset.y};
                circle.radius = circleCollider->radius;

                circleCollider->id = b2CreateCircleShape(rigidBody.id, &circleCollider->def, &circle);
            }
        });
    }

    void Scene::onRuntimeStop()
    {
        b2DestroyWorld(physicsWorldId);
        physicsWorldId = {};
    }
} // namespace mirras
