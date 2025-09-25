
// Using raylib's commit 3f6d67c61c59affa4561de2ddf988e487093bc80 (24/09/2025)

#include "raylib.h"
#include "math.h"

#define SPRITE_COUNT 100000

typedef struct Sprite
{
    float x, y;
    float vx, vy;
} Sprite;

static Sprite sprites[SPRITE_COUNT];
static Texture2D texture;

static int screenW = 0;
static int screenH = 0;
static bool startTest = false;

void update(float dt)
{
    if(!startTest)
        return;

    for(int i = 0; i < SPRITE_COUNT; ++i)
    {
        float width = texture.width;
        float height = texture.height;

        sprites[i].x += sprites[i].vx * dt;
        sprites[i].y += sprites[i].vy * dt;

        if(sprites[i].x < 0.f)
        {
            sprites[i].x = 0.f;
            sprites[i].vx *= -1;
        }
        else if(sprites[i].x + width > screenW)
        {
            sprites[i].x = screenW - width;
            sprites[i].vx *= -1;
        }

        if(sprites[i].y < 0.f)
        {
            sprites[i].y = 0.f;
            sprites[i].vy *= -1;
        }
        else if(sprites[i].y + height > screenH)
        {
            sprites[i].y = screenH - height;
            sprites[i].vy *= -1;
        }
    }
}

void draw()
{
    BeginDrawing();

        ClearBackground(BLACK);

        for(int i = 0; i < SPRITE_COUNT; ++i)
            DrawTextureV(texture, (Vector2){sprites[i].x, sprites[i].y}, WHITE);

    EndDrawing();
}

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(1280, 720, "raylib benchmark single texture");

    SetWindowState(FLAG_WINDOW_MAXIMIZED);

    screenW = GetScreenWidth();
    screenH = GetScreenHeight();

    SetTargetFPS(0); // No caps

    texture = LoadTexture("../icon.png");
    SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);

    const float pi = 3.141592f; // Use the same value across all frameworks
    const float speed = 100.f; // pixels / s

    for(int i = 0; i < SPRITE_COUNT; ++i)
    {
        float angle = (2.f * pi * i) / SPRITE_COUNT;

        sprites[i].x = screenW / 2;
        sprites[i].y = screenH / 2;
        sprites[i].vx = cosf(angle) * speed;
        sprites[i].vy = sinf(angle) * speed;
    }

    const float fixedTimeStep = 1.f / 60.f;
    float frameTimeAccumulator = 0.f;

    // Main game loop
    while(!WindowShouldClose())
    {
        // Check 'startTest' first so that IsKeyPressed is not called after the flag is set to true
        if(!startTest && IsKeyPressed(KEY_F11))
            startTest = true;

        const float frameTime = GetFrameTime();
        frameTimeAccumulator += frameTime;

        while(frameTimeAccumulator >= fixedTimeStep)
        {
            update(fixedTimeStep);
            frameTimeAccumulator -= fixedTimeStep;
        }

        draw();
    }

    UnloadTexture(texture);

    CloseWindow();

    return 0;
}
