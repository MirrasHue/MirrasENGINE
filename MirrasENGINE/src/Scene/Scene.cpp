#include "Scene/Scene.h"

#include "Core/Renderer/Renderer.h"
#include "Core/Log.h"

#include "Scene/Entity.h"
#include "Scene/Components.h"

namespace mirras
{
    Entity Scene::createEntity(UUID uuid)
    {
        Entity entity = {registry.create(), &registry};
        entity.add<IDComponent>(uuid);
        entity.add<TransformComponent>();

        return entity;
    }

    Entity Scene::createEntityWithHint(uint32 hint, UUID uuid)
    {
        Entity entity = {registry.create(entt::entity{hint}), &registry};
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
        registry.view<CppScriptComponent>().each([dt, this](auto entity, auto& cpp)
        {
            if(!cpp.script)
            {
                cpp.script = cpp.instantiateScript();
                cpp.script->entity = Entity{entity, &registry};
                cpp.script->onInit();
            }
            
            cpp.script->onUpdate(dt);
        });
    }

    void Scene::draw()
    {
        Camera2D* camera = nullptr;

        registry.view<TransformComponent, CameraComponent>().each([&camera](auto entity, auto& transform, auto& cameraComp)
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

    void Scene::draw(Camera2D& camera)
    {
        Renderer::beginMode2D(camera);

        // Sprites
        registry.view<TransformComponent, SpriteComponent>().each([](auto entity, auto& transform, auto& sprite)
        {
            if(!sprite.texture)
            {
                //ENGINE_LOG_ERROR("SpriteComponent has no texture assigned to it");
                return;
            }

            Renderer::setPixelOutputData((int32)entity);

            Renderer::drawTexture(*sprite.texture, sprite.texSampleArea, transform.position, transform.scale *
                                  glm::vec2{sprite.texture->width, sprite.texture->height}, {0.f, 0.f}, transform.rotation, sprite.tintColor);
        });

        // Rectangles
        registry.view<TransformComponent, RectangleComponent>().each([](auto entity, auto& transform, auto& rectangle)
        {
            Renderer::setPixelOutputData((int32)entity);

            Renderer::drawRectangle(transform.position, rectangle.size * transform.scale, {0.f, 0.f}, rectangle.color, transform.rotation);
        });

        // Circles
        registry.view<TransformComponent, CircleComponent>().each([](auto entity, auto& transform, auto& circle)
        {
            Renderer::setPixelOutputData((int32)entity);

            Renderer::drawShaderCircle(transform.position, circle.radius, circle.color, circle.fillFactor, circle.fadeFactor);
        });

        // Text
        registry.view<TransformComponent, TextComponent>().each([](auto entity, auto& transform, auto& text)
        {
            if(!text.font.atlasTexture)
            {
                //ENGINE_LOG_ERROR("TextComponent has no atlas texture assigned to font");
                return;
            }

            Renderer::setPixelOutputData((int32)entity);

            Renderer::drawText(text.text, text.font, transform.position, text.fontSize, text.color, text.kerning, text.lineSpacing);
        });

        Renderer::endMode2D();
    }

    void Scene::onEvent(Event& event)
    {
        
    }
} // namespace mirras
