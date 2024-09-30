#pragma once

#include "MirrasENGINE.h"

namespace mirras
{
    class EditorLayer : public Layer
    {
    public:
        virtual void load() {}
        virtual void update(float dt) {}
        virtual void onEvent(Event& event) override;
        virtual void draw() {}
        virtual void drawImGui() override;
        virtual void unload() {}
    };
} // namespace mirras
