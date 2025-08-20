#include "Backends/OpenGL/OpenGLShader.h"

#include "Core/Log.h"

#include "Utilities/FileIO.h"

#include <glad/glad.h>
#include <raylib/rlgl.h>

#include <glm/gtc/type_ptr.hpp>

namespace mirras
{
    OpenGLShader::OpenGLShader(std::string_view vertexSrc, std::string_view fragmentSrc)
    {
        init(vertexSrc, fragmentSrc);
    }

    OpenGLShader::OpenGLShader(const fs::path& vertexFilepath, const fs::path& fragmentFilepath)
    {
        std::string vertexSrc = file::read(vertexFilepath);
        std::string fragmentSrc = file::read(fragmentFilepath);

        init(vertexSrc, fragmentSrc);
    }

    void OpenGLShader::init(std::string_view vertexSrc, std::string_view fragmentSrc)
    {
        id = rlLoadShaderCode(vertexSrc.data(), fragmentSrc.data());
            
        // After shader loading, we try to set default location names

        // Default shader attribute locations have been bound before linking:
        //    vertex position location    = 0
        //    vertex texcoord location    = 1
        //    vertex normal location      = 2
        //    vertex color location       = 3
        //    vertex tangent location     = 4
        //    vertex texcoord2 location   = 5

        // Set all locations to -1 (no location)
        locs.resize(RL_MAX_SHADER_LOCATIONS, -1);

        // Get handles to GLSL input attribute locations
        locs[RL_SHADER_LOC_VERTEX_POSITION] = rlGetLocationAttrib(id, RL_DEFAULT_SHADER_ATTRIB_NAME_POSITION);
        locs[RL_SHADER_LOC_VERTEX_TEXCOORD01] = rlGetLocationAttrib(id, RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD);
        locs[RL_SHADER_LOC_VERTEX_TEXCOORD02] = rlGetLocationAttrib(id, RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD2);
        locs[RL_SHADER_LOC_VERTEX_NORMAL] = rlGetLocationAttrib(id, RL_DEFAULT_SHADER_ATTRIB_NAME_NORMAL);
        locs[RL_SHADER_LOC_VERTEX_TANGENT] = rlGetLocationAttrib(id, RL_DEFAULT_SHADER_ATTRIB_NAME_TANGENT);
        locs[RL_SHADER_LOC_VERTEX_COLOR] = rlGetLocationAttrib(id, RL_DEFAULT_SHADER_ATTRIB_NAME_COLOR);

        // Get handles to GLSL uniform locations (vertex shader)
        locs[RL_SHADER_LOC_MATRIX_MVP] = rlGetLocationUniform(id, RL_DEFAULT_SHADER_UNIFORM_NAME_MVP);
        locs[RL_SHADER_LOC_MATRIX_VIEW] = rlGetLocationUniform(id, RL_DEFAULT_SHADER_UNIFORM_NAME_VIEW);
        locs[RL_SHADER_LOC_MATRIX_PROJECTION] = rlGetLocationUniform(id, RL_DEFAULT_SHADER_UNIFORM_NAME_PROJECTION);
        locs[RL_SHADER_LOC_MATRIX_MODEL] = rlGetLocationUniform(id, RL_DEFAULT_SHADER_UNIFORM_NAME_MODEL);
        locs[RL_SHADER_LOC_MATRIX_NORMAL] = rlGetLocationUniform(id, RL_DEFAULT_SHADER_UNIFORM_NAME_NORMAL);

        // Get handles to GLSL uniform locations (fragment shader)
        locs[RL_SHADER_LOC_COLOR_DIFFUSE] = rlGetLocationUniform(id, RL_DEFAULT_SHADER_UNIFORM_NAME_COLOR);
        locs[RL_SHADER_LOC_MAP_DIFFUSE] = rlGetLocationUniform(id, RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE0);  // SHADER_LOC_MAP_ALBEDO
        locs[RL_SHADER_LOC_MAP_SPECULAR] = rlGetLocationUniform(id, RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE1); // SHADER_LOC_MAP_METALNESS
        locs[RL_SHADER_LOC_MAP_NORMAL] = rlGetLocationUniform(id, RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE2);
    }

    void OpenGLShader::makeActive()
    {        
        rlSetShader(id, locs.data());
    }

    void OpenGLShader::makeInactive()
    {
        rlSetShader(rlGetDefaultShaderId(), rlGetDefaultShaderLocs());
    }

    int32 OpenGLShader::getUniformLocation(std::string_view uniformName) const
    {
        return glGetUniformLocation(id, uniformName.data());
    }

    int32 OpenGLShader::getAttributeLocation(std::string_view attributeName) const
    {
        return glGetAttribLocation(id, attributeName.data());
    }

    bool OpenGLShader::findUniform(std::string_view name, int32& outLocation) const
    {
        outLocation = getUniformLocation(name);

        if(outLocation < 0)
        {
            LOG_WARN("Uniform '{}' not found in shader", name);
            return false;
        }
        
        rlEnableShader(id); // Enable the shader program before uploading uniform data
        return true;
    }

    bool OpenGLShader::isValid(int32 location) const
    {
        if(location < 0)
        {
            LOG_ERROR("Provided uniform location '{}' is not valid!", location);
            return false;
        }

        rlEnableShader(id);
        return true;
    }

    void OpenGLShader::setFloat(std::string_view name, float value)
    {
        int loc{};
        if(findUniform(name, loc))
            glUniform1f(loc, value);
    }

    void OpenGLShader::setFloat(int32 location, float value)
    {
        if(isValid(location))
            glUniform1f(location, value);
    }

    void OpenGLShader::setVec2(std::string_view name, glm::vec2 vec)
    {
        int loc{};
        if(findUniform(name, loc))
            glUniform2f(loc, vec.x, vec.y);
    }

    void OpenGLShader::setVec2(int32 location, glm::vec2 vec)
    {
        if(isValid(location))
            glUniform2f(location, vec.x, vec.y);
    }

    void OpenGLShader::setVec3(std::string_view name, const glm::vec3& vec)
    {
        int loc{};
        if(findUniform(name, loc))
            glUniform3f(loc, vec.x, vec.y, vec.z);
    }

    void OpenGLShader::setVec3(int32 location, const glm::vec3& vec)
    {
        if(isValid(location))
            glUniform3f(location, vec.x, vec.y, vec.z);
    }

    void OpenGLShader::setVec4(std::string_view name, const glm::vec4& vec)
    {
        int loc{};
        if(findUniform(name, loc))
            glUniform4f(loc, vec.x, vec.y, vec.z, vec.w);
    }

    void OpenGLShader::setVec4(int32 location, const glm::vec4& vec)
    {
        if(isValid(location))
            glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
    }

    void OpenGLShader::setMat4(std::string_view name, const glm::mat4& mat)
    {
        int loc{};
        if(findUniform(name, loc))
            glUniformMatrix4fv(loc, 1, false, glm::value_ptr(mat));
    }

    void OpenGLShader::setMat4(int32 location, const glm::mat4& mat)
    {
        if(isValid(location))
            glUniformMatrix4fv(location, 1, false, glm::value_ptr(mat));
    }

    void OpenGLShader::setInt(std::string_view name, int value)
    {
        int loc{};
        if(findUniform(name, loc))
            glUniform1i(loc, value);
    }

    void OpenGLShader::setInt(int32 location, int value)
    {
        if(isValid(location))
            glUniform1i(location, value);
    }

    void OpenGLShader::setIntArray(std::string_view name, std::span<int> values)
    {
        int loc{};
        if(findUniform(name, loc))
            glUniform1iv(loc, values.size(), values.data());
    }

    void OpenGLShader::setIntArray(int32 location, std::span<int> values)
    {
        if(isValid(location))
            glUniform1iv(location, values.size(), values.data());
    }

    OpenGLShader::~OpenGLShader()
    {
        if(id != rlGetDefaultShaderId() && id > 0)
            rlUnloadShaderProgram(id);
    }
} // namespace mirras
