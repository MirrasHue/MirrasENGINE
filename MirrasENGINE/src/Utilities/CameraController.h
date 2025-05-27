#pragma once

#include "Core/Renderer/Camera2D.h"
#include "Core/Fwd.h"

namespace mirras
{
    // TODO: find a better place for this class
    class CameraController
    {
    public:
        CameraController() = default;
        CameraController(Camera2D& _camera) :
            camera(_camera) {}

        void update(float dt);
        // Already called by update (Useful to apply zoom in the editor when the viewport is hovered but not focused)
        void updateZoom();
        void setCamera(Camera2D& cam) { camera = cam; }

    public:
        float speed = 100.f;
        float rotationSpeed = 90.f;
        float zoomStep = 0.5f; // Multiplied by the mouse wheel offset, in order to tweak the stepping

        bool rotate = true;
    
    private:
        Camera2D& camera;
    };
} // namespace mirras
