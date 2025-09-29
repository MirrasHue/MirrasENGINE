
#include "MirrasENGINE.h"

#include "Backends/OpenGL/OpenGLTexture.h"
#include "Events/KeyboardEvents.h"

#include <stb/image.h>

#include <cmath>

struct Sprite
{
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
    virtual void onEvent(mirras::Event& event) override;
    virtual void draw() override;

private:
    mirras::OpenGLTexture texture;
    std::array<Sprite, 100'000> sprites;
    int32 screenW{}, screenH{};
    bool startTest = false;
};

using namespace mirras;

void Benchmark::load()
{
    TextureSpecs specs;
    specs.data = stbi_load("assets/icon.png", &specs.width, &specs.height, &specs.channels, 0);

    texture.init(specs);
    
    screenW = App::getOSWindow().getInitialFbSize().x;
    screenH = App::getOSWindow().getInitialFbSize().y;

    constexpr float PI = 3.141592f; // Use the same value across all frameworks
    constexpr float speed = 100.f; // pixels / s

    for(size_t i = 0; i < sprites.size(); ++i)
    {
        auto& sprite = sprites[i];

        float angle = (2.f * PI * i) / sprites.size();

        sprite.x = screenW / 2;
        sprite.y = screenH / 2;
        sprite.vx = std::cos(angle) * speed;
        sprite.vy = std::sin(angle) * speed;
    }

    Renderer::setClearColor(0.f, 0.f, 0.f); // Black
}

void Benchmark::onEvent(Event& event)
{
    if(!Event::is_a<KeyPressed>(event))
        return;

    if(static_cast<KeyPressed&>(event).key == Key::F11)
        startTest = true;
}

void Benchmark::fixedUpdate(float dt)
{
    if(!startTest)
        return;

    for(auto& sprite : sprites)
    {
        float width = texture.width;
        float height = texture.height;

        sprite.x += sprite.vx * dt;
        sprite.y += sprite.vy * dt;

        if(sprite.x < 0.f)
        {
            sprite.x = 0.f;
            sprite.vx *= -1;
        }
        else if(sprite.x + width > screenW)
        {
            sprite.x = screenW - width;
            sprite.vx *= -1;
        }

        if(sprite.y < 0.f)
        {
            sprite.y = 0.f;
            sprite.vy *= -1;
        }
        else if(sprite.y + height > screenH)
        {
            sprite.y = screenH - height;
            sprite.vy *= -1;
        }
    }
}

void Benchmark::draw()
{
    for(const auto& sprite : sprites)
        Renderer::drawTexture(texture, {}, {sprite.x, sprite.y});
}

int main()
{
    mirras::AppSpecs appSpecs {
        .workingDirectory = "../../Samples/Benchmark",
        .fixedUpdateRate = 60
    };

    mirras::WindowSpecs windowSpecs {
        .title = "mirras benchmark single texture",
        .iconFilepath = "assets/icon.png",
        .maximized = true,
        .VSync = false
    };

    Benchmark app{appSpecs, windowSpecs};
    app.run();
}
