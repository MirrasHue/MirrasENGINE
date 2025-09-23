#include "Animation.h"

#include "Events/WindowEvents.h"

#include <format>

#define MAX_FRAME_SPEED 15
#define MIN_FRAME_SPEED  1

static constexpr glm::vec4 RED{1.f, 0.f, 0.f, 1.f};
static constexpr glm::vec4 GREEN{0.f, 0.9f, 0.f, 1.f};
static constexpr glm::vec4 BLUE{0.f, 0.f, 1.f, 1.f};
static constexpr glm::vec4 BLACK{0.f, 0.f, 0.f, 1.f};

using namespace mirras;

void Animation::load()
{
    scarfy = Texture::loadFrom("assets/scarfy.png");

    frameRect = {0.f, 0.f, scarfy->width / 6.f, (float)scarfy->height};

    font.loadFrom("assets/VT323-Regular.ttf");

    Renderer::setClearColor(1.f, 1.f, 1.f); // White
}

void Animation::fixedUpdate(float dt)
{
    framesCounter++;

    if(framesCounter >= 60 / framesSpeed)
    {
        framesCounter = 0;
        currentFrame++;

        if(currentFrame > 5) currentFrame = 0;

        frameRect.x = (float)currentFrame * scarfy->width / 6.f;
    }

    if(Input::isKeyDown(Key::Right))
        framesSpeed++;
    else
    if(Input::isKeyDown(Key::Left))
        framesSpeed--;

    if(framesSpeed > MAX_FRAME_SPEED)
        framesSpeed = MAX_FRAME_SPEED;
    else
    if(framesSpeed < MIN_FRAME_SPEED)
        framesSpeed = MIN_FRAME_SPEED;
}

void Animation::draw()
{
    float x = -scarfy->width / 2.f;
    float y = -scarfy->height * 1.5f;

    Renderer::beginMode2D(camera);
        Renderer::drawTexture(*scarfy, {}, {x, y});
        Renderer::drawRectangleLines({x, y}, {scarfy->width, scarfy->height}, GREEN);
        Renderer::drawRectangleLines({x + frameRect.x, y + frameRect.y}, {frameRect.width, frameRect.height}, RED);

        Renderer::drawText(L"FRAME SPEED", font, {-300.f, -30.f}, 25.f, BLACK);
        Renderer::drawText(std::format(L"{:02} FPS", framesSpeed), font, {240.f, -30.f}, 25.f, BLACK);
        Renderer::drawText(L"PRESS RIGHT/LEFT KEYS to CHANGE SPEED!", font, {-145.f, 0.f}, 22.f, BLACK);

        for (int i = 0; i < MAX_FRAME_SPEED; i++)
        {
            if (i < framesSpeed) Renderer::drawRectangle({-180.f + 27.f * i, -30.f}, {25.f, 25.f}, {}, RED);
            Renderer::drawRectangleLines({-180.f + 27.f * i, -30.f}, {25.f, 25.f}, RED);
        }

        Renderer::drawTexture(*scarfy, frameRect, {-frameRect.width / 2.f, 50.f}, {frameRect.width, frameRect.height});
        Renderer::drawText(L"(c) Scarfy sprite by Eiden Marsal", font, {120.f, frameRect.height + 70.f}, 20.f, BLACK);

    Renderer::endMode2D();
}
