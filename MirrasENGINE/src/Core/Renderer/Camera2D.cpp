#include "Core/Renderer/Camera2D.h"

#include "Core/Asserts.h"

#include "Events/WindowEvents.h"

namespace mirras
{
    Camera2D::Camera2D(glm::vec2 _position, glm::vec2 _offset, float _rotation, float _zoom)
        : position{_position}, offset{_offset}, rotation{_rotation}, zoom{_zoom} {}

    void Camera2D::targetSize(int32 framebufferWidth, int32 framebufferHeight)
    {
        MIRR_ASSERT_CORE(framebufferWidth > 0 && framebufferHeight > 0,
            "Invalid framebuffer size: {} x {}", framebufferWidth, framebufferHeight);

        auto [initialFbW, initialFbH] = Camera2D::currentFbInitialSize;

        zoomScale = (float)framebufferHeight / initialFbH;

        offsetX = offset.x / initialFbW * framebufferWidth;
        offsetY = offset.y / initialFbH * framebufferHeight;
    }
} // namespace mirras
