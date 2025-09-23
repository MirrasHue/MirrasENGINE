#include "PlaygroundApp.h"

using namespace mirras;

static constexpr glm::vec4 WHITE{1.f, 1.f, 1.f, 1.f};
static constexpr glm::vec4 BLACK{0.f, 0.f, 0.f, 1.f};

void PlaygroundApp::load()
{
    vertexSrc = R"(
        #version 330 core
    
        in vec3 vertexPosition;
        in vec2 vertexTexCoord;
        in vec4 vertexColor;

        out vec4 vertexPos;
        out vec2 fragTexCoord;
        out vec4 fragColor;

        uniform mat4 mvp;

        void main()
        {
            vertexPos = mvp * vec4(vertexPosition, 1.0);
            fragTexCoord = vertexTexCoord;
            fragColor = vertexColor; 
            gl_Position = mvp * vec4(vertexPosition, 1.0);
        }
    )";

    fragmentSrc = R"(
        #version 330 core
    
        in vec4 vertexPos;
        in vec2 fragTexCoord;
        in vec4 fragColor;
        
        out vec4 finalColor;

        uniform sampler2D texture0;
        uniform vec4 colDiffuse;

        void main()
        {
            finalColor = vertexPos;
            //vec4 texelColor = texture(texture0, fragTexCoord);
            //finalColor = texelColor * colDiffuse * fragColor;
        }
    )";
    
    shader = Shader::loadFrom(vertexSrc, fragmentSrc);
    camera.position = {400.f, 300.f};
    //camera.setOffset(-300.f, 100.f);

    points[0] = {triangleCenter.x - 100.f, triangleCenter.y + 100.f, 0.f};
    points[1] = {triangleCenter.x + 100.f, triangleCenter.y + 100.f, 0.f};
    points[2] = {triangleCenter.x, triangleCenter.y - 100.f, 0.f};

    texture = Texture::loadFrom("assets/textures/GuadagniniModel.png");
    font = instantiate<Font>("assets/fonts/Doto_Rounded-Black.ttf");

    sound1.loadFrom("assets/doodle_pop.ogg");
    sound2.loadFrom("assets/target.ogg");
    sound2.loop(true);
    sound3.loadFrom("assets/killdeer.wav");

    Renderer::setLineWidth(1.f);
}

void PlaygroundApp::onEvent(Event& event)
{
    
}

void PlaygroundApp::draw()
{
    rect4f sampleArea(0.f, 0.f, texture->width, texture->height);

    Renderer::beginMode2D(camera);
        Renderer::drawCircle({600.f, 400.f, 0.f}, 50.f, WHITE);

        Renderer::drawLine({0.f, 300.f}, {2000.f, 300.f}, WHITE);

        Renderer::drawTexture(*texture, sampleArea, {400.f, 300.f, 0.f}, {texture->width, texture->height}, {0.f, 0.f}, rotation);
        Renderer::drawShaderCircle({200.f, 300.f, 1.f}, 100.f, {1.f, 0.f, 0.5f, 1.f});

        shader->makeActive();
            Renderer::drawCircle({400.f, 300.f}, 100.f, {1.f, 0.f, 1.f, 1.f});
            Renderer::drawTriangle(points[0], points[1], points[2], BLACK);

            Renderer::drawRectangle({300.f, 400.f, 1.f}, {200.f, 200.f}, {100.f, 100.f}, BLACK, rotation);
        shader->makeInactive();

        Renderer::drawShaderCircle({300.f, 600.f, 0.f}, 200.f, {1.f, 1.f, 0.5f, 1.f}, 0.5f, 1.f);

        Renderer::drawTriangle({300.f, 400.f, 0.f}, {700.f, 400.f, 0.f}, {500.f, 100.f, 0.f}, {0.f, 0.f, 0.f, 0.5f});

        Renderer::drawText(L"Press Z, X or C to play a sound.\nPress Space to disable looping (sound 2)", *font, {0.f, 0.f, 0.f});

        if(font->atlasTexture)
            Renderer::drawTexture(*font->atlasTexture, {}, {0.f, 100.f, 0.f}, {330.f, 330.f});
        else
        {
            LOG_ERROR("font atlas is null");
        }

        // To visualize how the rendered glyphs are aligning with the specified top left position
        Renderer::drawLine({0.f, 450.f, 0.f}, {0.f, 600.f, 0.f}, WHITE);
        Renderer::drawLine({0.f, 450.f, 0.f}, {150.f, 450.f, 0.f}, WHITE);
        Renderer::drawText(L"Hello World!\nNow we have text rendering", *font, {0.f, 450.f, 0.f});

    Renderer::endMode2D();
}

void PlaygroundApp::update(float dt)
{
    rotation += 30.f * dt;

    if(rotation > 360.f)
        rotation -= 360.f;

    cameraController.update(dt);
    cameraController.updateZoom();

    if(Input::isKeyDown(Key::Z))
    {
        if(!sound1.isPlaying())
            sound1.play();
    }

    if(Input::isKeyDown(Key::X))
    {
        if(!sound2.isPlaying())
            sound2.play();
    }

    if(Input::isKeyDown(Key::C))
    {
        if(!sound3.isPlaying())
            sound3.play();
    }

    if(Input::isKeyDown(Key::Space))
    {
        sound2.loop(false);
    }
}

void PlaygroundApp::unload()
{
    
}
