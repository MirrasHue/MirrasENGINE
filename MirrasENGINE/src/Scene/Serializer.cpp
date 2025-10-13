#include "Scene/Serializer.h"

#include "Core/Log.h"

#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Utilities/Encodings.h"

#include <fkYAML/node.hpp>

#include <ranges>
#include <fstream>

namespace glm
{
    static void to_node(fkyaml::node& node, const vec2& v)
    {
        node = fkyaml::node::sequence({v.x, v.y});
    }

    static void from_node(const fkyaml::node& node, vec2& v)
    {
        v.x = node[0].get_value<float>();
        v.y = node[1].get_value<float>();
    }

    static void to_node(fkyaml::node& node, const vec3& v)
    {
        node = fkyaml::node::sequence({v.x, v.y, v.z});
    }

    static void from_node(const fkyaml::node& node, vec3& v)
    {
        v.x = node[0].get_value<float>();
        v.y = node[1].get_value<float>();
        v.z = node[2].get_value<float>();
    }

    static void to_node(fkyaml::node& node, const vec4& v)
    {
        node = fkyaml::node::sequence({v.r, v.g, v.b, v.a});
    }

    static void from_node(const fkyaml::node& node, vec4& v)
    {
        v.r = node[0].get_value<float>();
        v.g = node[1].get_value<float>();
        v.b = node[2].get_value<float>();
        v.a = node[3].get_value<float>();
    }
}

namespace mirras
{
    static void to_node(fkyaml::node& node, const TransformComponent& t)
    {
        node = fkyaml::node::mapping();
        node["Translation"] = t.position;
        node["Scale"] = t.scale;
        node["Rotation"] = t.rotation;
    }

    static void from_node(const fkyaml::node& node, TransformComponent& t)
    {
        t.position = node["Translation"].get_value<glm::vec3>();
        t.scale = node["Scale"].get_value<glm::vec2>();
        t.rotation = node["Rotation"].get_value<float>();
    }

    static void to_node(fkyaml::node& node, const RectangleComponent& r)
    {
        node = fkyaml::node::mapping();
        node["Color"] = r.color;
        node["Size"] = r.size;
    }

    static void from_node(const fkyaml::node& node, RectangleComponent& r)
    {
        r.color = node["Color"].get_value<glm::vec4>();
        r.size = node["Size"].get_value<glm::vec2>();
    }

    static void to_node(fkyaml::node& node, const CircleComponent& c)
    {
        node = fkyaml::node::mapping();
        node["Color"] = c.color;
        node["Radius"] = c.radius;
        node["Fill"] = c.fillFactor;
        node["Fade"] = c.fadeFactor;
    }

    static void from_node(const fkyaml::node& node, CircleComponent& c)
    {
        c.color = node["Color"].get_value<glm::vec4>();
        c.radius = node["Radius"].get_value<float>();
        c.fillFactor = node["Fill"].get_value<float>();
        c.fadeFactor = node["Fade"].get_value<float>();
    }

    static void to_node(fkyaml::node& node, const Camera2D& c)
    {
        node = fkyaml::node::mapping();
        node["Position"] = c.position;
        node["Offset"] = c.offset;
        node["Rotation"] = c.rotation;
        node["Zoom"] = c.zoom;
    }

    static void from_node(const fkyaml::node& node, Camera2D& c)
    {
        c.position = node["Position"].get_value<glm::vec2>();
        c.offset = node["Offset"].get_value<glm::vec2>();
        c.rotation = node["Rotation"].get_value<float>();
        c.zoom = node["Zoom"].get_value<float>();
    }

    static void to_node(fkyaml::node& node, const rect4f& r)
    {
        node = fkyaml::node::sequence({r.x, r.y, r.width, r.height});
    }

    static void from_node(const fkyaml::node& node, rect4f& r)
    {
        r.x = node[0].get_value<float>();
        r.y = node[1].get_value<float>();
        r.width = node[2].get_value<float>();
        r.height = node[3].get_value<float>();
    }

    static void to_node(fkyaml::node& node, const SpriteComponent& s)
    {
        node = fkyaml::node::mapping();
        node["Texture"] = s.imageFilepath;
        node["TextureFilter"] = (int32)s.textureFilter;
        node["SampleArea"] = s.texSampleArea;
        node["TintColor"] = s.tintColor;
    }

    static void from_node(const fkyaml::node& node, SpriteComponent& s)
    {
        auto filter = (TextureFilter)node["TextureFilter"].get_value<int32>();

        if(node["Texture"] != nullptr)
            s.loadTextureFrom(node["Texture"].get_value<std::string>(), filter);

        s.texSampleArea = node["SampleArea"].get_value<rect4f>();
        s.tintColor = node["TintColor"].get_value<glm::vec4>();
    }

    static void to_node(fkyaml::node& node, const TextComponent& c)
    {
        node = fkyaml::node::mapping();
        node["Font"] = c.fontFilepath;
        node["TextString"] = utf::toString(c.text);
        node["Color"] = c.color;
        node["FontSize"] = c.fontSize;
        node["Kerning"] = c.kerning;
        node["LineSpacing"] = c.lineSpacing;
    }

    static void from_node(const fkyaml::node& node, TextComponent& c)
    {
        if(node["Font"] != nullptr)
            c.loadFontFrom(node["Font"].get_value<std::string>());

        if(node["TextString"] != nullptr)
            c.text = utf::toU32string(node["TextString"].get_value<std::string>());

        c.color = node["Color"].get_value<glm::vec4>();
        c.fontSize = node["FontSize"].get_value<float>();
        c.kerning = node["Kerning"].get_value<float>();
        c.lineSpacing = node["LineSpacing"].get_value<float>();
    }

    static bool serializeEntity(Entity entity, fkyaml::node& node, uint32 idx)
    {
        if(!entity.has<IDComponent>())
        {
            // Casting entity handle to int32 so that if it's entt::null, -1 will be displayed (easier to debug)
            ENGINE_LOG_ERROR("Trying to serialize an entity without ID component. EnTT ID: {}", (int32)entity.handle);
            return false;
        }

        uint64 id = entity.get<IDComponent>().id;
        node["2|Entities"].as_seq().emplace_back(fkyaml::node::mapping()).add_tag_name("ID: " + std::to_string(id));

        if(entity.has<TagComponent>())
            node["2|Entities"][idx]["Tag"] = entity.get<TagComponent>().tag;

        if(entity.has<TransformComponent>())
            node["2|Entities"][idx]["Transform"] = entity.get<TransformComponent>();

        if(entity.has<CameraComponent>())
            node["2|Entities"][idx]["Camera"] = entity.get<CameraComponent>().camera;

        if(entity.has<SpriteComponent>())
            node["2|Entities"][idx]["Sprite"] = entity.get<SpriteComponent>();

        if(entity.has<RectangleComponent>())
            node["2|Entities"][idx]["Rectangle"] = entity.get<RectangleComponent>();

        if(entity.has<CircleComponent>())
            node["2|Entities"][idx]["Circle"] = entity.get<CircleComponent>();

        if(entity.has<TextComponent>())
            node["2|Entities"][idx]["Text"] = entity.get<TextComponent>();

        return true;
    }

    bool serialize(Scene& scene, const fs::path& filepath)
    {
        try
        {
            auto node = fkyaml::node::mapping();

            node["1|Scene"] = scene.name;
            node["2|Entities"] = fkyaml::node::sequence();

            uint32 entityIdx{};
            auto& registry = scene.registry;

            for(auto enttID : registry.view<entt::entity>() | std::views::reverse)
            {
                if(serializeEntity({enttID, &registry}, node, entityIdx))
                    entityIdx++;
            }

            std::ofstream out{filepath};
            out<<node;
        }
        catch(const std::exception& e)
        {
            ENGINE_LOG_ERROR("Failed to serialize scene: {}", e.what());
            return false;
        }

        return true;
    }

    bool deserialize(Scene& scene, const fs::path& filepath, uint32& entityCount)
    {
        uint32 count{};

        try
        {
            std::ifstream in(filepath);
            const auto node = fkyaml::node::deserialize(in);

            if(node["1|Scene"] == nullptr)
            {
                ENGINE_LOG_WARN("Invalid Scene file, not able do deserialize it");
                return false;
            }

            scene.name = node["1|Scene"].get_value<std::string>();

            for(const auto& entity : node["2|Entities"])
            {
                uint64 id{};

                if(entity["ID"] != nullptr)
                {
                    // fkYAML can't deserialize integers greater than 2^63 - 1, then they're treated as strings
                    if(entity["ID"].is_integer())
                        id = entity["ID"].get_value<uint64>();
                    else
                        id = std::stoull(entity["ID"].get_value<std::string>());
                }
                else
                {
                    ENGINE_LOG_WARN("Trying do deserialize an entity without ID");
                    continue;
                }
                    
                Entity deserialized = scene.createEntity(id);

                if(entity["Tag"] != nullptr)
                    deserialized.add<TagComponent>(entity["Tag"].get_value<std::string>());

                if(entity["Transform"] != nullptr) // Added by default when creating entity
                {
                    auto& transf = deserialized.get<TransformComponent>();
                    transf = entity["Transform"].get_value<TransformComponent>();
                }

                if(entity["Camera"] != nullptr)
                    deserialized.add<CameraComponent>(entity["Camera"].get_value<Camera2D>());

                if(entity["Sprite"] != nullptr)
                    deserialized.add<SpriteComponent>(entity["Sprite"].get_value<SpriteComponent>());

                if(entity["Rectangle"] != nullptr)
                    deserialized.add<RectangleComponent>(entity["Rectangle"].get_value<RectangleComponent>());

                if(entity["Circle"] != nullptr)
                    deserialized.add<CircleComponent>(entity["Circle"].get_value<CircleComponent>());

                if(entity["Text"] != nullptr)
                    deserialized.add<TextComponent>(entity["Text"].get_value<TextComponent>());

                count++;
            }
        }
        catch(const std::exception& e)
        {
            ENGINE_LOG_ERROR("Failed to deserialize scene: {}", e.what());
            entityCount = count;
            return false;
        }

        entityCount = count;
        return true;
    }
} // namespace mirras
