#pragma once

#include "MirrasENGINE.h"

class SceneExample : public mirras::Layer
{
public:
    virtual void load() override;
    virtual void update(float dt) override;
    virtual void onEvent(mirras::Event& event) override;
    virtual void draw() override;
    virtual void drawImGui() override;
    virtual void unload() override {}

public:
    mirras::Scene scene;
};