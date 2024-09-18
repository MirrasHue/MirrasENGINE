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
}

void PlaygroundLayer::onEvent(mirras::Event& event)
{
    camera.onWindowResize(event);
}

void PlaygroundLayer::draw()
{
    mirras::Renderer::beginMode2D(camera);
        shader->makeActive();

        mirras::Renderer::drawTriangle();

        shader->makeInactive();
    mirras::Renderer::endMode2D();
}

void PlaygroundLayer::drawImGui()
{
    ImGui::ShowDemoWindow();
}
