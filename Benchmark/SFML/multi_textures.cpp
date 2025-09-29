
// Using SFML's commit dbe4f499ba2a290f1ff0b8b21e5352484f7300fa (22/09/2025)

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Err.hpp>

// For printing the window size before the program ends, in order to check if it's really 1920 x 991
//#include <iostream> // This is commented for running the benchmark
#include <array>
#include <cmath>

struct Sprite
{
    sf::Texture texture;
    float x{}, y{};
    float vx{}, vy{};
};

class Benchmark
{
public:
    void run();
    void update(float dt);
    void draw();

private:
    sf::RenderWindow window;
    std::array<Sprite, 10'000> sprites;
    uint32_t screenW{}, screenH{};
    bool startTest = false;
};

void Benchmark::run()
{
    // Currently there's no way to programmatically maximize the window, so we have to do it manually
    // We know that the maximized window will be 1920 x 991 (with 125% scale), so we set it here beforehand
    window.create(sf::VideoMode({1920, 991}), "SFML benchmark multi textures");

    screenW = window.getSize().x;
    screenH = window.getSize().y;    

    sf::Image img("../icon.png");

    constexpr float PI = 3.141592f; // Use the same value across all frameworks
    constexpr float speed = 100.f; // pixels / s

    for(size_t i = 0; i < sprites.size(); ++i)
    {
        auto& sprite = sprites[i];

        float angle = (2.f * PI * i) / sprites.size();

        if(!sprite.texture.loadFromImage(img))
        {
            sf::err()<<"Could not create textures\n";
            return;
        }

        sprite.x = screenW / 2;
        sprite.y = screenH / 2;
        sprite.vx = std::cos(angle) * speed;
        sprite.vy = std::sin(angle) * speed;
    }

    sf::Clock clock;
    sf::Time frameTimeAccumulator;
    constexpr auto fixedTimeStep = sf::seconds(1.f / 60.f);

    while(window.isOpen())
    {
        while(const std::optional event = window.pollEvent())
        {
            if(event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else
            if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if(keyPressed->scancode == sf::Keyboard::Scan::F11)
                    startTest = true;
            }
        }

        auto frameTime = clock.restart();
        frameTimeAccumulator += frameTime;

        while(frameTimeAccumulator >= fixedTimeStep)
        {
            update(fixedTimeStep.asSeconds());
            frameTimeAccumulator -= fixedTimeStep;
        }

        draw();
    }

    /*auto [width, height] = window.getSize();
    std::cout<<width<<" x "<<height<<'\n';*/
}

void Benchmark::update(float dt)
{
    if(!startTest)
        return;

    for(auto& sprite : sprites)
    {
        auto [width, height] = sprite.texture.getSize();

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
    window.clear();

    for(const auto& sprite : sprites)
    {
        sf::Sprite temp(sprite.texture);
        temp.setPosition({sprite.x, sprite.y});

        window.draw(temp);
    }

    window.display();
}

int main()
{
    Benchmark benchmark;
    benchmark.run();
}
