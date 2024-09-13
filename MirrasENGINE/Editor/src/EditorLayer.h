#pragma once

#include "MirrasENGINE.h"
#include "Core/Renderer/Camera2D.h"

namespace mirras
{
    class EditorLayer : public Layer
    {
    public:
        virtual void load() {}
        virtual void update(float dt) {}
        virtual void onEvent(Event& event) override;
        virtual void draw() override;
        virtual void drawImGui() override;
        virtual void unload() {}

        Camera2D camera{{400, 300}, {0, -200}}; // Temp
    };
}
    