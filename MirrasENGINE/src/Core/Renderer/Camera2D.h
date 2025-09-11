#pragma once

#include "Core/Types/Basic.h"
#include "Core/Fwd.h"

#include <glm/detail/type_vec2.hpp>
#include <glm/fwd.hpp>

namespace mirras
{
    class Camera2D
    {
    public:
        Camera2D() = default;
        Camera2D(glm::vec2 position, glm::vec2 offset = {}, float rotation = 0.f, float zoom = 1.f);

        glm::mat4 getViewMatrix(vec2i framebufferSize, vec2i initialFbSize) const;

    public:
        glm::vec2 position; // Aligns with the center of the screen (when the offset is {0, 0})
        glm::vec2 offset;

        float rotation = 0.f; // In degrees
        float zoom = 1.f;
    };
} // namespace mirras
