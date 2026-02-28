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
        if(paused)
            return;

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
        if(paused)
            return;

        const int32 subSteps = 4;
        b2World_Step(physicsWorldID, ts, subSteps);

        //b2BodyEvents events = b2World_GetBodyEvents(physicsWorldID);
        registry.view<TransformComponent, RigidBodyComponent>().each([this](auto enttID, auto& transform, auto& rigidBody)
        {
            auto bodyTransf = b2Body_GetTransform(rigidBody.id);
            transform.position.x = bodyTransf.p.x * pixelsPerMeter;
            transform.position.y = bodyTransf.p.y * pixelsPerMeter;
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

    // For testing. Consider wether to display physics colliders here or in the editor layer
    /*static void b2LineSeg(b2Vec2 p1, b2Vec2 p2, b2HexColor, void*)
    {
        Renderer::drawLine({p1.x, p1.y}, {p2.x, p2.y}, glm::vec4{1.f});
    }

    static void b2PolyLines(b2Transform transform, const b2Vec2* vertices, int vertexCount, float radius, b2HexColor, void*)
    {
        if(vertexCount != 4)
            return;

        // TODO: modify this so that it's not set globally (setting back to 1 before returning doesn't work, and it 
        // would be better to restore the current line width, but requires to keep it saved to avoid querying OpenGL)
        Renderer::setLineWidth(3.f);
        //glm::vec2 pos = glm::vec2{transform.p.x, transform.p.y} * 100.f;
        //Renderer::drawCircle(glm::vec3{pos, 0.f}, 20.f, glm::vec4{1.f});

        b2Vec2 p1 = b2TransformPoint(transform, vertices[3]);
        for(int i = 0; i < vertexCount; ++i)
        {
            b2Vec2 p2 = b2TransformPoint(transform, vertices[i]);
            glm::vec2 v1 = glm::vec2{p1.x, p1.y} * 100.f;
            glm::vec2 v2 = glm::vec2{p2.x, p2.y} * 100.f;
            Renderer::drawLine(v1, v2, glm::vec4{1.f});
            p1 = p2;
        }
    }

    static b2DebugDraw initDebugDraw()
    {
        b2DebugDraw d = b2DefaultDebugDraw();

        d.DrawLineFcn = b2LineSeg;
        d.DrawSolidPolygonFcn = b2PolyLines;

        return d;
    }

    static bool operator!=(b2WorldId id1, b2WorldId id2)
    {
        return (id1.index1 != id2.index1) || (id1.generation != id2.generation);
    }*/

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
            glm::vec2 size = rectangle.size * transform.scale;
            glm::vec3 center = transform.position - glm::vec3{size / 2.f, 0.f};
            Renderer::drawRectangle(center, size, size * 0.5f, rectangle.color, transform.rotation);
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

        // For testing. Consider wether to display physics colliders here (with b2World_Draw)
        // or in the editor layer (iterating through the colliders in the registry)
        /*if(physicsWorldID != b2WorldId{})
        {
            static b2DebugDraw debug = initDebugDraw();
            b2World_Draw(physicsWorldID, &debug);
        }*/

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

        registry.view<ScriptComponent>().each([](auto enttID, auto& script)
        {
            if(!script.instance.hasData())
                return;

            sol::table data = script.instance.env[".data"];
            sol::protected_function fn = data["on_init"];

            if(!fn.valid())
                return;

            sol::set_environment(script.instance.env, fn);
            auto result = fn(data);

            if(!result.valid())
            {
                sol::error error = result;
                ENGINE_LOG_ERROR("Error while executing Lua script [on_init()]: {}", error.what());
            }
        });

        // TODO: each scene should have its own b2WorldDef that can be configured in the editor
        b2WorldDef def = b2DefaultWorldDef();
        def.gravity.y *= -1;

        physicsWorldID = b2CreateWorld(&def);

        registry.view<TransformComponent, RigidBodyComponent>().each([this](auto enttID, auto& transform, auto& rigidBody)
        {
            Entity entity{enttID, &registry};

            rigidBody.def.position.x = transform.position.x / pixelsPerMeter;
            rigidBody.def.position.y = transform.position.y / pixelsPerMeter;
            rigidBody.def.rotation = b2MakeRot(glm::radians(transform.rotation));

            rigidBody.id = b2CreateBody(physicsWorldID, &rigidBody.def);

            auto* boxCollider = entity.tryGet<BoxColliderComponent>();

            if(boxCollider)
            {
                glm::vec2 halfSize = boxCollider->size * (0.5f / pixelsPerMeter);
                glm::vec2 offset = boxCollider->offset / (float)pixelsPerMeter;

                b2Polygon box = b2MakeOffsetBox(halfSize.x, halfSize.y, {offset.x, offset.y}, b2Rot{1.f, 0.f});                

                boxCollider->id = b2CreatePolygonShape(rigidBody.id, &boxCollider->def, &box);
            }

            auto* circleCollider = entity.tryGet<CircleColliderComponent>();

            if(circleCollider)
            {
                glm::vec2 offset = circleCollider->offset / (float)pixelsPerMeter;

                b2Circle circle;
                circle.center = {offset.x, offset.y};
                circle.radius = circleCollider->radius / pixelsPerMeter;

                circleCollider->id = b2CreateCircleShape(rigidBody.id, &circleCollider->def, &circle);
            }

            auto massData = b2Body_GetMassData(rigidBody.id);
            massData.mass = rigidBody.mass;
            b2Body_SetMassData(rigidBody.id, massData);
        });
    }

    void Scene::onRuntimeStop()
    {
        b2DestroyWorld(physicsWorldID);
        physicsWorldID = {};
    }
} // namespace mirras
