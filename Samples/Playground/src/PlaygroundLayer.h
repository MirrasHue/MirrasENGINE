#pragma once

#include "MirrasENGINE.h"

#include "Core/Renderer/Camera2D.h"
#include "Core/Renderer/Shader.h"

class PlaygroundLayer : public mirras::Layer
{
public:
    virtual void load() override;
    virtual void update(float dt) {}
    virtual void onEvent(mirras::Event& event) override;
    virtual void draw() override;
    virtual void drawImGui() override;
    virtual void unload() {}

public:
    mirras::Camera2D camera{{400, 300}, {-300, 150}};
    std::string_view vertexSrc, fragmentSrc;
    mirras::single_ref<mirras::Shader> shader;
};