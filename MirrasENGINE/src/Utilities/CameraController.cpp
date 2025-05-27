#include "Utilities/CameraController.h"

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

        updateZoom();
    }

    void CameraController::updateZoom()
    {
        camera.zoom += Input::mouseWheelScroll.y * zoomStep;
        camera.zoom = std::max(camera.zoom, 0.5f);
    }
} // namespace mirras
