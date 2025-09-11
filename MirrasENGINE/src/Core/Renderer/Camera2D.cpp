#include "Core/Renderer/Camera2D.h"

#include "Core/Asserts.h"

#include "Events/WindowEvents.h"

#include <glm/gtc/matrix_transform.hpp>

namespace mirras
{
    Camera2D::Camera2D(glm::vec2 _position, glm::vec2 _offset, float _rotation, float _zoom)
        : position{_position}, offset{_offset}, rotation{_rotation}, zoom{_zoom} {}

    glm::mat4 Camera2D::getViewMatrix(vec2i framebufferSize, vec2i initialFbSize) const
    {
        MIRR_ASSERT_CORE(framebufferSize.x > 0 && framebufferSize.y > 0,
            "Invalid framebuffer size: {} x {}", framebufferSize.x, framebufferSize.y);

        const float zoomScale = (float)framebufferSize.y / initialFbSize.y;
        const float offsetX = offset.x / initialFbSize.x * framebufferSize.x;
        const float offsetY = offset.y / initialFbSize.y * framebufferSize.y;

        static const glm::mat4 identity = glm::mat4{1.f};
        const float appliedZoom = zoom * zoomScale;

        return glm::translate(identity, glm::vec3{framebufferSize.x/2.f - offsetX, framebufferSize.y/2.f - offsetY, 0.f}) *
               glm::rotate(identity, glm::radians(rotation), glm::vec3{0.f, 0.f, 1.f}) *
               glm::scale(identity, glm::vec3{appliedZoom, appliedZoom, 1.f}) *
               glm::translate(identity, glm::vec3{-position.x, -position.y, 0.f});
    }
} // namespace mirras
