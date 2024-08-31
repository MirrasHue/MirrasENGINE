#pragma once

#include "Input/DeviceCodes.h"

namespace mirras
{
    class Input
    {
    public:
        static bool  isKeyDown(Key key);
        static bool  isMouseButtonDown(Mouse button);
    };
}