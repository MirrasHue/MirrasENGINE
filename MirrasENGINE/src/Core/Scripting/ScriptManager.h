#pragma once

#include "Core/Types/Basic.h"
#include "Core/Fwd.h"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/state.hpp>
#include <sol/environment.hpp>

#include <unordered_map>
#include <filesystem_fs>

namespace mirras
{
    struct ScriptInstance
    {
        sol::environment env;

        bool hasData() { return env.valid() && env[".data"].valid(); }
    };

    class ScriptManager
    {
    public:
        static void init();
        static ScriptInstance loadScriptFrom(const fs::path& scriptFilepath, const Entity& entity);

    private:
        inline static sol::state lua;
        inline static std::unordered_map<std::u8string, ScriptInstance> loadedScripts;
    };
} // namespace mirras
