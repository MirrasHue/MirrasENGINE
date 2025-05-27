#pragma once

#include "MirrasENGINE.h"

namespace mirras
{
    class EditorLayer : public Layer
    {
    public:
        virtual void load() override;
        virtual void update(float dt) override;
        virtual void onEvent(Event& event) override;
        virtual void draw() override;
        virtual void drawImGui() override;
        virtual void unload() {}

    private:
        Camera2D editorCamera;
        CameraController cameraController{editorCamera};
        RenderTexture2D canvas{1280, 720};
        glm::vec2 viewportSize{1280, 720};

        float fontSize = 17.f; // Used by ImGui text

        bool viewportFocused = false;
        bool viewportHovered = false;
    };
} // namespace mirras
