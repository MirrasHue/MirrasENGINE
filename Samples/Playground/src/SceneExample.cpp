#include "SceneExample.h"

#include <imgui/imgui.h>

#include <msdf/msdf-atlas-gen.h>

class CameraScript : public mirras::EntityScript
{
public:
    void onInit() override
    {
        // This is not good because the reference can be invalidated if the registry resizes
        // One solution is to get the camera every frame, I hope there's a better way to do this
        //camera = &getComponent<mirras::CameraComponent>().camera;
    }

    void onUpdate(float dt) override
    {
        camera = &getComponent<mirras::CameraComponent>().camera;

        float speed = 100.f * dt;

        if(mirras::Input::isKeyDown(mirras::Key::D))
            camera->position.x += speed;
        if(mirras::Input::isKeyDown(mirras::Key::A))
            camera->position.x -= speed;
        if(mirras::Input::isKeyDown(mirras::Key::W))
            camera->position.y -= speed;
        if(mirras::Input::isKeyDown(mirras::Key::S))
            camera->position.y += speed;
    }

private:
    mirras::Camera2D* camera = nullptr;
};

void SceneExample::load()
{
    auto cameraEntity = scene.createEntity();
    auto& camera = cameraEntity.add<mirras::CameraComponent>();
    cameraEntity.addCppScript<CameraScript>();

    auto guadagnini = scene.createEntity();
    auto& sprite = guadagnini.add<mirras::SpriteComponent>();
    sprite.texture = mirras::Texture::loadFrom("assets/textures/GuadagniniModel.png");

    auto textEntity = scene.createEntity();
    auto& text = textEntity.add<mirras::TextComponent>();
    text.font = mirras::instantiate<mirras::Font>("assets/fonts/consolas.ttf");
    text.text = L"Hello World!";
}

void SceneExample::onEvent(mirras::Event& event)
{
    scene.onEvent(event);
}

void SceneExample::draw()
{
    scene.draw();
}

void SceneExample::update(float dt)
{
    scene.update(dt);
}

void SceneExample::drawImGui()
{
    ImGui::ShowDemoWindow();
}
