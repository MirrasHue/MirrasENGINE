#pragma once

#include "MirrasENGINE.h"

#include "Panels/SceneHierarchyPanel.h"

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
        single_ref<Scene> scene;
        RenderTexture2D canvas{1280, 720};
        glm::vec2 size{1280, 720};
        Camera2D camera; // To be used in editing mode
        SceneState state = SceneState::Editing;
        bool focused = false;
        bool hovered = false;
        bool hidden = true;
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
        virtual void unload() override {}

    private:
        std::vector<EditorScene> scenes;
        Scene* activeScene = nullptr;

        CameraController cameraController;
        CameraController zoomController; // For separately zooming on scenes that are hovered

        SceneHierarchyPanel sceneHierarchy;

        float fontSize = 17.f; // Used by ImGui text
    };
} // namespace mirras
