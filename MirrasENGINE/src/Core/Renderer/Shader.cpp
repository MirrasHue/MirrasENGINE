#include "Core/Renderer/Shader.h"

#include "Core/Renderer/Renderer.h"
#include "Core/Asserts.h"

#include "Backends/OpenGL/OpenGLShader.h"

namespace mirras
{
    single_ref<Shader> Shader::loadFrom(std::string_view vertexSrc, std::string_view fragmentSrc)
    {
        switch(Renderer::getBackend())
        {
            case Renderer::Backend::OpenGL:
                return instantiate<OpenGLShader>(vertexSrc, fragmentSrc);
        }

        MIRR_ASSERT_CORE(false, "Unexpected renderer backend!");
        return {};
    }

    single_ref<Shader> Shader::loadFrom(const fs::path& vertexFilepath, const fs::path& fragmentFilepath)
    {
        return instantiate<OpenGLShader>(vertexFilepath, fragmentFilepath);
    }
} // namespace mirras
