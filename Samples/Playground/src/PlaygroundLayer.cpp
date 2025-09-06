#include "PlaygroundLayer.h"

#include <imgui/imgui.h>

void PlaygroundLayer::load()
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
    
    shader = mirras::Shader::loadFrom(vertexSrc, fragmentSrc);
    camera.position = {400, 300};
    //camera.setOffset(-300, 100);

    points[0] = {triangleCenter.x - 100, triangleCenter.y + 100, 0.f};
    points[1] = {triangleCenter.x + 100, triangleCenter.y + 100, 0.f};
    points[2] = {triangleCenter.x, triangleCenter.y - 100, 0.f};

    texture = mirras::Texture::loadFrom("assets/textures/GuadagniniModel.png");
    font.loadFrom("assets/fonts/Doto_Rounded-Black.ttf");

    mirras::Renderer::setLineWidth(1.f);
    sound1.loadFrom("assets/doodle_pop.ogg");
    sound2.loadFrom("assets/target.ogg");
    sound2.loop(true);
    sound3.loadFrom("assets/killdeer.wav");
}

void PlaygroundLayer::onEvent(mirras::Event& event)
{
    
}

void PlaygroundLayer::draw()
{
    mirras::rect4f sampleArea(0, 0, texture->width, texture->height);

    mirras::Renderer::beginMode2D(camera);
        mirras::Renderer::drawCircle({600, 400, 0}, 50, {1,1,1,1});

        mirras::Renderer::drawLine({0, 300}, {2000, 300}, {1,1,1,1});

        mirras::Renderer::drawTexture(*texture, sampleArea, {400, 300, 0}, {texture->width, texture->height}, {0,0}, rotation);
        mirras::Renderer::drawShaderCircle({200, 300, 1}, 100, {1,0,0.5,1});

        shader->makeActive();
            mirras::Renderer::drawCircle({400, 300}, 100, {1,0,1,1});
            mirras::Renderer::drawTriangle(points[0], points[1], points[2], {0,0,0,1});

            mirras::Renderer::drawRectangle({300, 400, 1}, {200, 200}, {100, 100}, {0,0,0,1}, rotation);
        shader->makeInactive();

        mirras::Renderer::drawShaderCircle({300, 600, 0}, 200, {1,1,0.5,1}, 0.5, 1.f);

        mirras::Renderer::drawTriangle({300, 400, 0}, {700, 400, 0}, {500, 100, 0}, {0,0,0,0.5});

        if(font.atlasTexture)
            mirras::Renderer::drawTexture(*font.atlasTexture, {}, {0, 100, 0}, {330,330});
        else
        {
            LOG_ERROR("font atlas is null");
        }

        // To visualize how the rendered glyphs are aligning with the specified top left position
        mirras::Renderer::drawLine({0,450,0}, {0,600,0}, {1,1,1,1});
        mirras::Renderer::drawLine({0,450,0}, {150,450,0}, {1,1,1,1});
        mirras::Renderer::drawText(L"Hello World!\nNow we have text rendering", font, {0,450,0});

    mirras::Renderer::endMode2D();
}

void PlaygroundLayer::update(float dt)
{
    rotation += 30.f * dt;

    if(rotation > 720.f)
        rotation -= 720.f;

    cameraController.update(dt);
    cameraController.updateZoom();

    if(mirras::Input::isKeyDown(mirras::Key::Z))
    {
        if(!sound1.isPlaying())
            sound1.play();
    }

    if(mirras::Input::isKeyDown(mirras::Key::X))
    {
        if(!sound2.isPlaying())
            sound2.play();
    }

    if(mirras::Input::isKeyDown(mirras::Key::C))
    {
        if(!sound3.isPlaying())
            sound3.play();
    }

    if(mirras::Input::isKeyDown(mirras::Key::Space))
    {
        sound2.loop(false);
    }
}

void PlaygroundLayer::drawImGui()
{
    ImGui::ShowDemoWindow();
}
