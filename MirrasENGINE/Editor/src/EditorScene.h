#pragma once

#include "Core/Renderer/Camera2D.h"
#include "Core/Renderer/Texture.h"
#include "Core/Types/Integer.h"
#include "Core/Types/Reference.h"

#include "Scene/Scene.h"

#include <glm/vec2.hpp>

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
        RenderTextureEx canvas{1280, 720};
        glm::vec2 size{1280, 720}; // ImGui viewport size
        Camera2D camera; // To be used in editing mode
        Entity selectedEntity;
        uint32 addedEntityCount = 0; // Incremented when adding entities through the right click ctx menu
        SceneState state = SceneState::Editing;
        bool focused = false;
        bool hovered = false;
        bool open = true;
    };
} // namespace mirras
