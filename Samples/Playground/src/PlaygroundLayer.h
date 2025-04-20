#pragma once

#include "MirrasENGINE.h"

#include <glm/gtc/matrix_transform.hpp>

class PlaygroundLayer : public mirras::Layer
{
public:
    virtual void load() override;
    virtual void update(float dt) override;
    virtual void onEvent(mirras::Event& event) override;
    virtual void draw() override;
    virtual void drawImGui() override;
    virtual void unload() {}

public:
    mirras::Camera2D camera;
    mirras::CameraController cameraController{camera};
    mirras::single_ref<mirras::Texture> texture;
    float rotation{};
    std::string_view vertexSrc, fragmentSrc;
    mirras::single_ref<mirras::Shader> shader;
    glm::vec2 triangleCenter{400, 300};
    std::array<glm::vec3, 3> points;
    mirras::single_ref<mirras::Font> font;
};