#pragma once

#include "Core/Renderer/Shader.h"

namespace mirras
{
    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader(std::string_view vertexSrc, std::string_view fragmentSrc);
        OpenGLShader(const fs::path& vertexFilepath, const fs::path& fragmentFilepath);

        virtual void makeActive();
        virtual void makeInactive();

        virtual int32 getUniformLocation(std::string_view uniformName) const;
        virtual int32 getAttributeLocation(std::string_view attributeName) const;
        
        virtual void setFloat(std::string_view name, float value);
        virtual void setFloat(int32 location, float value);

        virtual void setVec2(std::string_view name, const glm::vec2& vec);
        virtual void setVec2(int32 location, const glm::vec2& vec);

        virtual void setVec3(std::string_view name, const glm::vec3& vec);
        virtual void setVec3(int32 location, const glm::vec3& vec);

        virtual void setVec4(std::string_view name, const glm::vec4& vec);
        virtual void setVec4(int32 location, const glm::vec4& vec);

        virtual void setMat4(std::string_view name, const glm::mat4& mat);
        virtual void setMat4(int32 location, const glm::mat4& mat);
        
        virtual void setInt(std::string_view name, int value);
        virtual void setInt(int32 location, int value);

        virtual void setIntArray(std::string_view name, std::span<int> values);
        virtual void setIntArray(int32 location, std::span<int> values);

    private:
        void init(std::string_view vertexSrc, std::string_view fragmentSrc);
        bool findUniform(std::string_view name, int32& outLocation) const;
        bool isValid(int32 location) const;

    public:
        uint32 id;
        std::vector<int32> locs;
    };
}