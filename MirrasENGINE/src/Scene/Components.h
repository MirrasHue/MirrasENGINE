#pragma once

#include "Core/Renderer/Texture.h"
#include "Core/Renderer/Camera2D.h"
#include "Core/Renderer/Font.h"
#include "Core/Types/Reference.h"

#include "Utilities/UUID.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace mirras
{
    // As this is intended to be used only for 2D, we don't need a vec3 for scale and rotation
    // Also, given the way raylib is setup, it would be more difficult to use this as a full transform 
    struct TransformComponent
    {
        glm::vec3 position{0.f};
        glm::vec2 scale{1.f};
        float rotation{};
    };

    struct CameraComponent
    {
        Camera2D camera;
    };

    struct IDComponent
    {
        UUID id;
    };

    struct TagComponent
    {
        std::string tag;
    };

    struct SpriteComponent
    {
        single_ref<Texture> texture;
        rect4f texSampleArea;
        glm::vec4 color{1.f};
    };

    struct RectangleComponent
    {
        glm::vec4 color{1.f};
        glm::vec2 size{100.f, 100.f};
    };

    struct CircleComponent
    {
        glm::vec4 color{1.f};
        float radius{50.f};
        float fillFactor{1.f};
        float fadeFactor{0.007f};
    };

    struct TextComponent
    {
        single_ref<Font> font;
        std::wstring text;
        glm::vec4 color{1.f};
        float fontSize{40.f};
        float kerning{0.f};
        float lineSpacing{0.f};
    };

    struct CppScriptComponent
    {
        single_ref<EntityScript> script;
        
        //std::function<single_ref<EntityScript>()> instantiateScript;
        single_ref<EntityScript>(*instantiateScript)();
    };

} // namespace mirras
