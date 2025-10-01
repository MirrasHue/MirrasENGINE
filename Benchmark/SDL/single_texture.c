
// Using SDL's commit d42bf59c669628d13dbc8cd41be93f2216211ff9 (28/09/2025)

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <math.h>

#define SPRITE_COUNT 10000

typedef struct Sprite
{
    SDL_Texture *texture;
    float x, y;
    float vx, vy;
} Sprite;

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static Sprite sprites[SPRITE_COUNT];

static const float fixedTimeStep = 1.f / 60.f;
static bool startTest = false;
static Uint64 lastTick = 0;
static int screenW = 0;
static int screenH = 0;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    if(!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "0"); // No limits, call SDL_AppIterate() as fast as possible
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    SDL_SetHint(SDL_HINT_RENDER_VSYNC,       "0"); // No VSync

    if(!SDL_CreateWindowAndRenderer("SDL benchmark single texture", 1280, 720, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_GetWindowSizeInPixels(window, &screenW, &screenH);

    SDL_Surface *surface = SDL_LoadBMP("../icon.bmp");
    if(!surface) {
        SDL_Log("Couldn't load bitmap: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Treat white as transparent, as we had to convert the PNG to BMP, to avoid pulling more dependencies (SDL_Image)
    SDL_SetSurfaceColorKey(surface, true, SDL_MapSurfaceRGB(surface, 255, 255, 255));

    const float PI = 3.141592f; // Use the same value across all frameworks
    const float speed = 100.f; // pixels / s

    bool error = false;

    for(int i = 0; i < SPRITE_COUNT; ++i)
    {
        float angle = (2.f * PI * i) / SPRITE_COUNT;

        sprites[i].texture = SDL_CreateTextureFromSurface(renderer, surface);
        
        if(!sprites[i].texture) {
            error = true;
            continue;
        }

        SDL_SetTextureScaleMode(sprites[i].texture, SDL_SCALEMODE_LINEAR);

        sprites[i].x = screenW / 2;
        sprites[i].y = screenH / 2;
        sprites[i].vx = cosf(angle) * speed;
        sprites[i].vy = sinf(angle) * speed;
    }

    SDL_DestroySurface(surface);

    if(error)
    {
        SDL_Log("Couldn't create static textures: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black

    lastTick = SDL_GetTicks();

    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if(event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    if(event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_F11)
        startTest = true;

    return SDL_APP_CONTINUE;
}

static void update(float dt)
{
    if(!startTest)
        return;

    for(int i = 0; i < SPRITE_COUNT; ++i)
    {
        float width = sprites[i].texture->w;
        float height = sprites[i].texture->h;

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

static void draw()
{
    SDL_RenderClear(renderer);

    for(int i = 0; i < SPRITE_COUNT; ++i)
    {
        SDL_FRect dest;
        dest.x = sprites[i].x;
        dest.y = sprites[i].y;
        dest.w = sprites[i].texture->w;
        dest.h = sprites[i].texture->h;
        SDL_RenderTexture(renderer, sprites[i].texture, NULL, &dest);
    }

    SDL_RenderPresent(renderer);
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    static float frameTimeAccumulator = 0.f;

    const Uint64 now = SDL_GetTicks();
    const float frameTime = (now - lastTick) / 1000.f;
    lastTick = now;

    frameTimeAccumulator += frameTime;

    while(frameTimeAccumulator >= fixedTimeStep)
    {
        update(fixedTimeStep);
        frameTimeAccumulator -= fixedTimeStep;
    }

    draw();

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    for(int i = 0; i < SPRITE_COUNT; ++i)
        SDL_DestroyTexture(sprites[i].texture);
}
