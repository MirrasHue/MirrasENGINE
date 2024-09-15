#pragma once

#include "Core/BasicTypes.h"

namespace mirras
{
    class Camera2D;

    class OpenGLRenderer
    {
    public:
        void init();

        void setViewport(int32 x, int32 y, int32 width, int32 height);
        
        void clearBackBuffers();
        void setClearColor(float r, float g, float b, float a = 1.f);

        void beginDrawing();
        void endDrawing();

        void beginMode2D(Camera2D& camera);
        void endMode2D();

        void drawTriangle(); // Temp, used for testing the camera

        ~OpenGLRenderer();
    };
}