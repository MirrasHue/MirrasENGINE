#include "Core/Scripting/ScriptManager.h"

#include "Core/Log.h"
#include "Scene/Entity.h"
#include "Utilities/Encodings.h"

#include <sol/sol.hpp>

namespace mirras
{
    // FNV-1a hash algorithm, Wikipedia
    // Relies on null terminator, so don't use this for vector<int>, e.g.
    /*template<typename Char_t>
    static uint64 hashString(const Char_t* str)
    {
        uint64 hash = 14695981039346656037ull;

        for(uint64 i = 0; str[i]; ++i)
            hash = (hash ^ str[i]) * 1099511628211ull;

        return hash;
    }*/

    void ScriptManager::init()
    {
        lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math);

        sol::table mirrasModule = lua.create_table();

        // We could use sol::variadic_args, fmt::dynamic_format_arg_store and fmt::vformat in order
        // to have `fmt` like formatting in Lua. It would require parsing the argument list and memory
        // allocations (for 1+ args). Doing the formatting with Lua itself is probably more performant
        mirrasModule.set_function("log_trace", [](std::string_view str){ LOG_TRACE(str); });
        mirrasModule.set_function("log_info",  [](std::string_view str){ LOG_INFO(str);  });
        mirrasModule.set_function("log_warn",  [](std::string_view str){ LOG_WARN(str);  });
        mirrasModule.set_function("log_error", [](std::string_view str){ LOG_ERROR(str); });

        // These will be moved to another place later, and more will be added
        lua.new_usertype<glm::vec2>("vec2",
            sol::call_constructor,
            sol::constructors<glm::vec2(float), glm::vec2(float, float)>(),
            "x", &glm::vec2::x,
            "y", &glm::vec2::y
        );

        lua.new_usertype<glm::vec3>("vec3",
            sol::call_constructor,
            sol::constructors<glm::vec3(float), glm::vec3(float, float, float)>(),
            "x", &glm::vec3::x,
            "y", &glm::vec3::y,
            "z", &glm::vec3::z,
            // For testing purposes, to account for the lack of += operator (this will be reworked)
            "add_x", [](glm::vec3& self, float x){ self.x += x; }
        );

        lua.new_usertype<TransformComponent>("Transform",
            "position", &TransformComponent::position,
            "scale", &TransformComponent::scale,
            "rotation", &TransformComponent::rotation
        );

        lua.new_usertype<Camera2D>("Camera2D",
            "position", &Camera2D::position,
            "offset", &Camera2D::offset,
            "rotation", &Camera2D::rotation,
            "zoom", &Camera2D::zoom
        );

        lua["package"]["loaded"]["MirrasENGINE"] = mirrasModule;

        // Adapted from: https://stackoverflow.com/questions/640642/how-do-you-copy-a-lua-table-by-value
        lua.script(R"(
            function deep_copy(obj, seen)
                local seen = seen or {}

                -- Handle non-tables and previously-seen tables
                if type(obj) ~= 'table' then
                    return obj
                end
                if seen[obj] then
                    return seen[obj]
                end

                -- New table, mark it as seen and copy recursively
                local copy = {}
                seen[obj] = copy

                for k, v in next, obj, nil do
                    copy[deep_copy(k, seen)] = deep_copy(v, seen)
                end

                return setmetatable(copy, getmetatable(obj)) -- Not copying the metatable, we'll see if it's needed later
            end
        )");
    }    

    // We could modify this and put in the global Lua namespace, and just set the
    // entity when calling on_update, etc. (saves more memory, only one instance)
    // As it stands, these entity functions won't be available globally, so if they
    // end up being called from the global namespace, the script execution will fail
    static sol::table createEntityFunctionsTable(Entity entity, sol::environment& env)
    {
        sol::table components = env.create();

        components.set_function("get_transform", [entity]()
        {
            auto* transf = entity.tryGet<TransformComponent>();
            //auto pos = transf->position;
            //ENGINE_LOG_ERROR("entity: {}  pos: {} {} {}", entity.get<TagComponent>().tag, pos.x, pos.y, pos.z);
            
            // In the documentation this case isn't covered, so I'm not sure if it's considered as bad as just
            // returning nullptr. It worked fine from Lua, if the component doesn't exist, I get a nil value there
            return transf ? transf : std::nullptr_t{};
        });

        components.set_function("get_camera", [entity]()
        {
            auto* cam = entity.tryGet<CameraComponent>();
            return cam ? &cam->camera : nullptr_t{};
        });

        components.set_function("get_tag", [entity]()
        {
            auto* tag = entity.tryGet<TagComponent>();
            return tag ? &tag->tag : nullptr_t{};
        });

        components.set_function("get_", [entity]()
        {
            
        });

        return components;
    }

    ScriptInstance ScriptManager::loadScriptFrom(const fs::path& scriptFilepath, const Entity& entity)
    {
        auto u8path = scriptFilepath.u8string();
        auto it = loadedScripts.find(u8path);

        if(it != loadedScripts.end())
        {
            ScriptInstance instance;
            instance.env = sol::environment(lua, sol::create, it->second.env);
            sol::table data = it->second.env[".data"];

            if(data.valid())
                instance.env[".data"] = lua["deep_copy"](data);

            instance.env["entity"] = createEntityFunctionsTable(entity, instance.env);

            return instance;
        }

        auto env = sol::environment(lua, sol::create, lua.globals());
        auto result = lua.script_file(utf::toString(u8path), env, sol::script_pass_on_error); // Don't throw on failure

        if(!result.valid())
        {
            sol::error error = result;
            ENGINE_LOG_ERROR("Failed to run Lua script [{}]: {}", scriptFilepath.string(), error.what());

            return {};
        }

        ScriptInstance instance;
        instance.env = sol::environment(lua, sol::create, env);
        instance.env["entity"] = createEntityFunctionsTable(entity, instance.env);

        // Kind of a data section, to store the result returned from the script. It's going to be easier to expose this in the editor
        if(sol::table data = result; data.valid())
        {
            env[".data"] = data;
            instance.env[".data"] = lua["deep_copy"](data);
        }

        auto& cache = loadedScripts[u8path];
        cache.env = env;

        return instance;
    }
} // namespace mirras
