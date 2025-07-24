#pragma once

#include "MirrasENGINE.h"

#include "EditorScene.h"

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/EntityPropertiesPanel.h"

#include <vector>

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
        virtual void unload() override {}

    private:
        std::vector<EditorScene> scenes;
        EditorScene* activeScene = nullptr;

        CameraController cameraController;
        CameraController zoomController; // For separately zooming on scenes that are hovered

        SceneHierarchyPanel sceneHierarchy;
        EntityPropertiesPanel entityProperties;

        float fontSize = 17.f; // Used by ImGui text
    };
} // namespace mirras
