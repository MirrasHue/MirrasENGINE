#pragma once

#include "Core/BasicTypes.h"

namespace mirras
{
    class Camera2D
    {
    public:
        Camera2D() = default;
        Camera2D(vec2f position, vec2f offset = {}, float rotation = 0.f, float zoom = 1.f);

        void setOffset(float xOffset, float yOffset);
        void setOffsetX(float xOffset);
        void setOffsetY(float yOffset);

        vec2f getOffset() { return offset; }

        void onWindowResize(class Event& event);

    public:
        vec2f position; // Aligns with the center of the screen (when the offset is 0,0)
        
        float rotation = 0.f; // In degrees
        float zoom = 1.f;

    private:
        // The ratios need to be recalculated when the offset changes, so rendering scales correctly when the window resizes,
        vec2f offset; // that's why offset is not accessed directly
        vec2f offsetRatio; // With respect to the initial window size
    };
}