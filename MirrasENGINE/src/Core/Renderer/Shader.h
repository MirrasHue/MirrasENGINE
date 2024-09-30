#pragma once

#include "Core/BasicTypes.h"
#include "Core/Fwd.h"

#include <filesystem>
#include <string_view>
#include <span>

#include <glm/fwd.hpp>

namespace mirras
{
    class Shader
    {
    public:
        virtual ~Shader() = default;

        virtual void makeActive()   = 0;
        virtual void makeInactive() = 0;

        virtual int32 getUniformLocation(std::string_view uniformName) const = 0;
        virtual int32 getAttributeLocation(std::string_view attributeName) const = 0;
        
        virtual void setFloat(std::string_view name, float value) = 0;
        virtual void setFloat(int32 location, float value) = 0;

        virtual void setVec2(std::string_view name, glm::vec2 vec) = 0;
        virtual void setVec2(int32 location, glm::vec2 vec) = 0;

        virtual void setVec3(std::string_view name, const glm::vec3& vec) = 0;
        virtual void setVec3(int32 location, const glm::vec3& vec) = 0;

        virtual void setVec4(std::string_view name, const glm::vec4& vec) = 0;
        virtual void setVec4(int32 location, const glm::vec4& vec) = 0;

        virtual void setMat4(std::string_view name, const glm::mat4& mat) = 0;
        virtual void setMat4(int32 location, const glm::mat4& mat) = 0;
        
        virtual void setInt(std::string_view name, int value) = 0;
        virtual void setInt(int32 location, int value) = 0;

        virtual void setIntArray(std::string_view name, std::span<int> values) = 0;
        virtual void setIntArray(int32 location, std::span<int> values) = 0;

        static single_ref<Shader> loadFrom(std::string_view vertexSrc, std::string_view fragmentSrc);
        static single_ref<Shader> loadFrom(const fs::path& vertexFilepath, const fs::path& fragmentFilepath);
        //static single_ref<Shader> loadFrom(const fs::path& shaderFilepath);
    };
} // namespace mirras
