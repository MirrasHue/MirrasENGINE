#pragma once

#include "MirrasENGINE.h"

#include "Core/Renderer/Camera2D.h"
#include "Core/Renderer/Shader.h"

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
    float rotation{};
    std::string_view vertexSrc, fragmentSrc;
    mirras::single_ref<mirras::Shader> shader;
    glm::vec2 triangleCenter{400, 300};
    std::array<glm::vec3, 3> points;
};