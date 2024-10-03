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
    camera.setOffset(-300, 100);

    points[0] = {triangleCenter.x - 100, triangleCenter.y + 100, 0.f};
    points[1] = {triangleCenter.x + 100, triangleCenter.y + 100, 0.f};
    points[2] = {triangleCenter.x, triangleCenter.y - 100, 0.f};

    texture = mirras::Texture::loadFrom("sprites.png"); //Temp
}

void PlaygroundLayer::onEvent(mirras::Event& event)
{
    camera.onWindowResize(event);
}

void PlaygroundLayer::draw()
{
    mirras::rect4i sampleArea = {0, 0, texture->width, texture->height};

    mirras::Renderer::beginMode2D(camera);
        mirras::Renderer::drawTexture(*texture, {}, {400, 300, 0}, {texture->width, texture->height}, {0,0}, rotation);

        shader->makeActive();
            mirras::Renderer::drawTriangle(points[0], points[1], points[2], {0,0,0,1});

            mirras::Renderer::drawRectangle({300, 400, 1}, {200, 200}, {100, 100}, {0,0,0,1}, rotation);
        shader->makeInactive();

        mirras::Renderer::drawTriangle({300, 400, 0}, {700, 400, 0}, {500, 100, 0}, {0,0,0,0.5});
    mirras::Renderer::endMode2D();
}

void PlaygroundLayer::update(float dt)
{
    rotation += 30 * dt;

    if(mirras::Input::isKeyDown(mirras::Key::D))
    {
        triangleCenter.x += 200 * dt;

        points[0] = {triangleCenter.x - 100, triangleCenter.y + 100, 0.f};
        points[1] = {triangleCenter.x + 100, triangleCenter.y + 100, 0.f};
        points[2] = {triangleCenter.x, triangleCenter.y - 100, 0.f};

        camera.position = {triangleCenter.x, triangleCenter.y};
    }
}

void PlaygroundLayer::drawImGui()
{
    ImGui::ShowDemoWindow();
}
