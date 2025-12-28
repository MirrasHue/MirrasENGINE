#pragma once

#include "Core/Renderer/Shader.h"
#include "Core/Fwd.h"

#include <vector>

namespace mirras
{
    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader() = default;
        OpenGLShader(std::string_view vertexSrc, std::string_view fragmentSrc);
        OpenGLShader(const fs::path& vertexFilepath, const fs::path& fragmentFilepath);

        void init(std::string_view vertexSrc, std::string_view fragmentSrc);
        void unload(); // So that shaders that are static or global can be cleaned up before the OpenGL context is destroyed

        virtual void makeActive()   override;
        virtual void makeInactive() override;

        virtual int32 getUniformLocation(std::string_view uniformName) const override;
        virtual int32 getAttributeLocation(std::string_view attributeName) const override;
        
        virtual void setFloat(std::string_view name, float value) override;
        virtual void setFloat(int32 location, float value) override;

        virtual void setVec2(std::string_view name, glm::vec2 vec) override;
        virtual void setVec2(int32 location, glm::vec2 vec) override;

        virtual void setVec3(std::string_view name, const glm::vec3& vec) override;
        virtual void setVec3(int32 location, const glm::vec3& vec) override;

        virtual void setVec4(std::string_view name, const glm::vec4& vec) override;
        virtual void setVec4(int32 location, const glm::vec4& vec) override;

        virtual void setMat4(std::string_view name, const glm::mat4& mat) override;
        virtual void setMat4(int32 location, const glm::mat4& mat) override;
        
        virtual void setInt(std::string_view name, int value) override;
        virtual void setInt(int32 location, int value) override;

        virtual void setIntArray(std::string_view name, std::span<int> values) override;
        virtual void setIntArray(int32 location, std::span<int> values) override;

        virtual ~OpenGLShader();

    private:
        bool findUniform(std::string_view name, int32& outLocation) const;
        bool isValid(int32 location) const;

    public:
        uint32 id;
        std::vector<int32> locs;
    };
} // namespace mirras
