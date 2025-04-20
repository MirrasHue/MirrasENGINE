#pragma once

#include "Core/Fwd.h"

namespace mirras
{
    // TODO: find a better place for this class
    class CameraController
    {
    public:
        CameraController() = default;
        CameraController(Camera2D& camera2D) :
            camera(camera2D) {}

        void update(float dt);
        void onMouseWheelScroll(Event& event);

    public:
        float speed = 100.f;
        float rotationSpeed = 90.f;
        float zoomStep = 0.5f; // Multiplied by the mouse wheel offset, in order to tweak the stepping

        bool rotate = true;
    
    private:
        Camera2D& camera;
    };
} // namespace mirras
