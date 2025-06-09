#pragma once

#include "MirrasENGINE.h"

#include <vector>

namespace mirras
{
    enum class SceneState : uint8
    {
        Editing,
        Playing
    };

    struct EditorScene
    {
        shared_ref<Scene> scene;
        RenderTexture2D canvas{1280, 720};
        glm::vec2 size{1280, 720};
        Camera2D camera; // To be used in editing mode
        SceneState state = SceneState::Editing;
        bool focused = false;
        bool hovered = false;
        bool hidden = false;
        bool open = true;
    };

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
        CameraController cameraController;
        CameraController camControllerHovered; // For zooming on scenes that are hovered

        std::vector<EditorScene> scenes;
        shared_ref<Scene> activeScene;

        float fontSize = 17.f; // Used by ImGui text

        //bool viewportFocused = false;
        //bool viewportHovered = false;
    };
} // namespace mirras
