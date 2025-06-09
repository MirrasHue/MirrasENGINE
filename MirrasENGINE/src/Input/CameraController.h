#pragma once

#include "Core/Fwd.h"

namespace mirras
{
    class CameraController
    {
    public:
        CameraController() = default;
        CameraController(Camera2D* _camera) :
            camera(_camera) {}

        void update(float dt);
        // Zoom needs to be updated separately because in the editor we want to apply zoom
        void updateZoom(); // only on scenes that are hovered instead of focused (in editing mode)
        void setCamera(Camera2D* cam) { camera = cam; }

    public:
        float speed = 100.f;
        float rotationSpeed = 90.f;
        float zoomStep = 0.5f; // Multiplied by the mouse wheel offset, in order to tweak the stepping

        bool rotate = true;
    
    private:
        Camera2D* camera = nullptr;
    };
} // namespace mirras
