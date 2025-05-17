#pragma once

#include "Core/Fwd.h"
#include "Core/Types/Basic.h"

#include <glm/detail/type_vec2.hpp>
#include <glm/fwd.hpp>

namespace mirras
{
    class Camera2D
    {
    public:
        Camera2D() = default;
        Camera2D(glm::vec2 position, glm::vec2 offset = {}, float rotation = 0.f, float zoom = 1.f);

        // Already called for us by the engine, no need to call it ourselves
        void targetSize(int32 framebufferWidth, int32 framebufferHeight);

    public:
        glm::vec2 position; // Aligns with the center of the screen (when the offset is {0, 0})
        glm::vec2 offset;

        float rotation = 0.f; // In degrees
        float zoom = 1.f;

        // Reflects the initial size of the default framebuffer or render texture (the one which is
        // currently being used). This is used to calculate the camera offsets and zoom scale correctly
        inline static vec2i currentFbInitialSize;

    private:
        float zoomScale = 1.f; // To account for window vertical resizes
        float offsetX = 0.f;
        float offsetY = 0.f;

        friend glm::mat4 getCameraMatrix(const Camera2D& camera);
    };
} // namespace mirras
