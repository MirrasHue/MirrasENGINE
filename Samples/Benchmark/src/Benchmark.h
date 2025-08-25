#pragma once

#include "MirrasENGINE.h"

#include "Backends/OpenGL/OpenGLTexture.h"

struct Sprite
{
    //mirras::OpenGLTexture texture;
    float x{}, y{};   // position
    float vx{}, vy{}; // velocity
};

class Benchmark : public mirras::App
{
public:
    Benchmark(const mirras::AppSpecs& appSpecs, const mirras::WindowSpecs& windowSpecs) :
        App{appSpecs, windowSpecs} {}

protected:
    virtual void load() override;
    virtual void fixedUpdate(float dt) override;
    //virtual void update(float dt) override;
    virtual void onEvent(mirras::Event& event) override;
    virtual void draw() override;
    //virtual void unload() override;

private:
    mirras::OpenGLTexture texture;
    std::array<Sprite, 100'000> sprites;
    int32 screenW{}, screenH{};
    bool startTest = false;
};
