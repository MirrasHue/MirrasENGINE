#pragma once

#include "MirrasENGINE.h"

#include <glm/vec3.hpp>

class PlaygroundApp : public mirras::App
{
public:
    PlaygroundApp(const mirras::AppSpecs& appSpecs, const mirras::WindowSpecs& windowSpecs) :
        App{appSpecs, windowSpecs} {}

    virtual void load() override;
    virtual void update(float dt) override;
    virtual void onEvent(mirras::Event& event) override;
    virtual void draw() override;
    virtual void unload() override;

public:
    mirras::Camera2D camera;
    mirras::CameraController cameraController{&camera};
    mirras::single_ref<mirras::Texture> texture;
    float rotation{};
    std::string_view vertexSrc, fragmentSrc;
    mirras::single_ref<mirras::Shader> shader;
    glm::vec2 triangleCenter{400, 300};
    std::array<glm::vec3, 3> points;
    mirras::single_ref<mirras::Font> font;
    mirras::Sound sound1;
    mirras::Sound sound2;
    mirras::Sound sound3;
};
