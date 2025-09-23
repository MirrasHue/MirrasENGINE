#pragma once

#include "MirrasENGINE.h"

// Adapted from raylib's sprite anim example

class Animation : public mirras::App
{
public:
    Animation(const mirras::AppSpecs& appSpecs, const mirras::WindowSpecs& windowSpecs) :
        App{appSpecs, windowSpecs} {}

protected:
    virtual void load() override;
    virtual void fixedUpdate(float dt) override;
    //virtual void onEvent(mirras::Event& event) override;
    virtual void draw() override;
    //virtual void unload() override;

private:
    mirras::Camera2D camera;
    mirras::single_ref<mirras::Texture> scarfy;
    mirras::Font font;
    mirras::rect4f frameRect;
    int32 currentFrame = 0;
    int32 framesCounter = 0;
    int32 framesSpeed = 8;
};
