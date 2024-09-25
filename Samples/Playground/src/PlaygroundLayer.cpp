#include "PlaygroundLayer.h"


#include <imgui/imgui.h>

void PlaygroundLayer::load()
{
    vertexSrc = R"(
        #version 330 core
    
        in vec3 vertexPosition;

        out vec4 vertexPos;

        uniform mat4 mvp;

        void main()
        {
            vertexPos = mvp * vec4(vertexPosition, 1.0);
            gl_Position = vertexPos;
        }
    )";

    fragmentSrc = R"(
        #version 330 core
    
        in vec4 vertexPos;
        
        out vec4 finalColor;

        void main()
        {
            finalColor = vertexPos;
        }
    )";
    
    shader = mirras::Shader::loadFrom(vertexSrc, fragmentSrc);
    camera.position = {400, 300};
    camera.setOffset(-300, 100);

    points[0] = {triangleCenter.x - 100, triangleCenter.y + 100, 0.f};
    points[1] = {triangleCenter.x + 100, triangleCenter.y + 100, 0.f};
    points[2] = {triangleCenter.x, triangleCenter.y - 100, 0.f};
}

void PlaygroundLayer::onEvent(mirras::Event& event)
{
    camera.onWindowResize(event);
}

void PlaygroundLayer::draw()
{
    mirras::Renderer::beginMode2D(camera);

        shader->makeActive();

            mirras::Renderer::drawTriangle(points[0], points[1], points[2], {});
            mirras::Renderer::drawRectangle({800, 100, 1}, {200, 200}, {100, 100}, {}, rotation);

        shader->makeInactive();

        mirras::Renderer::drawTriangle({300, 400}, {700, 400}, {500, 100}, {0.5,0.5,1,1});
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
