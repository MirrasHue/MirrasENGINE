#pragma once

#include "Core/Types/Basic.h"

#include "Input/DeviceCodes.h"

namespace mirras
{
    class Input
    {
    public:
        Input() = delete;

        static bool isKeyDown(Key key);
        static bool isMouseButtonDown(Mouse button);
    
    public:
        inline static vec2f mouseWheelScroll;
    };
} // namespace mirras
