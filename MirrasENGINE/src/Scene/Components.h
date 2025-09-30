#pragma once

#include "Core/Renderer/Texture.h"
#include "Core/Renderer/Camera2D.h"
#include "Core/Renderer/Font.h"
#include "Core/Types/Reference.h"

#include "Utilities/UUID.h"

#include <glm/gtc/matrix_transform.hpp>

#include <filesystem_fs>
#include <cmath>

namespace mirras
{
    // As this is intended to be used only for 2D, we don't need a vec3 for scale and rotation
    struct TransformComponent
    {
        glm::vec3 position{0.f};
        glm::vec2 scale{1.f};
        float rotation{};

        glm::mat4 getTransformMatrix() const;
        void decomposeTransform(const glm::mat4& transform);
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
        glm::vec4 tintColor{1.f};
        std::string imageFilepath;
        TextureFilter textureFilter;

        void loadTextureFrom(const fs::path& imageFilepath, TextureFilter filter = TextureFilter::Linear);
    };

    struct RectangleComponent
    {
        glm::vec4 color{1.f};
        glm::vec2 size{200.f, 200.f};
    };

    struct CircleComponent
    {
        glm::vec4 color{1.f};
        float radius{100.f};
        float fillFactor{1.f};
        float fadeFactor{0.007f};
    };

    struct TextComponent
    {
        Font font;
        std::string text;
        glm::vec4 color{1.f};
        std::string fontFilepath;
        float fontSize{40.f};
        float kerning{0.f};
        float lineSpacing{0.f};

        void loadFontFrom(const fs::path& fontFilepath);
    };

    struct CppScriptComponent
    {
        single_ref<EntityScript> script;
        
        //std::function<single_ref<EntityScript>()> instantiateScript;
        single_ref<EntityScript>(*instantiateScript)();
    };


    inline glm::mat4 TransformComponent::getTransformMatrix() const
    {
        static glm::mat4 identity{1.f};

        return glm::translate(identity, position) *
               glm::rotate(identity, glm::radians(rotation), glm::vec3{0.f, 0.f, 1.f}) *
               glm::scale(identity, glm::vec3{scale, 1.f});
    }

    // Adapted from https://www.reddit.com/r/opengl/comments/sih6lc/4x4_matrix_to_position_rotation_and_scale
    // Thank you AndreiDespinoiu, you spared me from using glm::decompose() XD
    inline void TransformComponent::decomposeTransform(const glm::mat4& transform)
    {
        // Translation is stored in the last column
        position = transform[3];

        glm::vec2 col0 = transform[0]; // X axis
        glm::vec2 col1 = transform[1]; // Y axis

        scale = {glm::length(col0), glm::length(col1)};

        //if(scale.x != 0.f) col0 /= scale.x; // The normalization doesn't seem necessary

        // Rotation around Z axis
        rotation = glm::degrees(std::atan2(col0.y, col0.x));
    }

    inline void SpriteComponent::loadTextureFrom(const fs::path& filepath, TextureFilter filter)
    {
        texture = Texture::loadFrom(filepath, filter);

        if(texture->id > 0)
        {
            imageFilepath = filepath.generic_string();
            textureFilter = filter;
        }
    }

    inline void TextComponent::loadFontFrom(const fs::path& filepath)
    {
        if(font.loadFrom(filepath))
            fontFilepath = filepath.generic_string();
    }
} // namespace mirras
