#pragma once

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

        void setOffset(float xOffset, float yOffset);
        void setOffsetX(float xOffset);
        void setOffsetY(float yOffset);

        glm::vec2 getOffset() const { return offset; }

        void onWindowResize(Event& event);

    public:
        glm::vec2 position; // Aligns with the center of the screen (when the offset is 0,0)

        float rotation = 0.f; // In degrees
        float zoom = 1.f;

    private:
        // The ratios need to be recalculated when the offset changes, so the rendering scales appropriately when
        glm::vec2 offset; // the window resizes, that's why offset is not accessed directly.
        glm::vec2 offsetRatio; // With respect to the initial window size

        float zoomScale = 1.f; // To account for window vertical resizes

        friend glm::mat4 getCameraMatrix(const Camera2D& camera);
    };
} // namespace mirras
