#pragma once

#include "Core/Fwd.h"

namespace mirras
{
    class Layer
    {
    public:
        virtual void load() {}
        virtual void update(float dt) {}
        virtual void onEvent(Event& event) {}
        virtual void draw() {}
        virtual void drawImGui() {}
        virtual void unload() {}

        virtual ~Layer() = default;
    };
} // namespace mirras
