#include "Utilities/CameraController.h"

#include "Core/Renderer/Camera2D.h"

#include "Events/MouseEvents.h"

#include "Input/Input.h"

namespace mirras
{
    void CameraController::update(float dt)
    {
        if(Input::isKeyDown(Key::D))
            camera.position.x += speed * dt;
        if(Input::isKeyDown(Key::A))
            camera.position.x -= speed * dt;
        if(Input::isKeyDown(Key::W))
            camera.position.y -= speed * dt;
        if(Input::isKeyDown(Key::S))
            camera.position.y += speed * dt;

        if(!rotate)
            return;
            
        if(Input::isKeyDown(Key::Q))
            camera.rotation -= rotationSpeed * dt;
        if(Input::isKeyDown(Key::E))
            camera.rotation += rotationSpeed * dt;
    }

    void CameraController::onMouseWheelScroll(Event& event)
    {
        if(!Event::is_a<MouseWheelScrolled>(event))
            return;

        auto[xOffset, yOffset] = static_cast<MouseWheelScrolled&>(event).mouseWheelOffset;

        camera.zoom += yOffset * zoomStep;
        camera.zoom = std::max(camera.zoom, 0.5f);
    }
} // namespace mirras
