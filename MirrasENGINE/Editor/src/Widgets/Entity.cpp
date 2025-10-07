#include "Widgets/Entity.h"

#include "Utilities/Encodings.h"

#include <glm/gtc/type_ptr.hpp>

static constexpr ImVec4 RED{1.f, 0.f, 0.f, 1.f};
static constexpr ImVec4 GREEN{0.f, 0.9f, 0.f, 1.f};
static constexpr ImVec4 BLUE{0.f, 0.f, 1.f, 1.f};
static constexpr ImVec4 GRAY{0.78f, 0.78f, 0.78f, 1.f}; // Light gray

namespace mirras
{
    struct ControlParams
    {
        // Drag float
        float min = 0.f;
        float max = 0.f;
        float speed = 0.1f;
        int32 flags = 0;

        // Button
        float resetValue = 0.f;
        float textAlignX = 0.58f; // For better centering X, Y and Z inside the button across different font sizes
    };

    static void drawControl(const char* label, float& value, const ImVec4& color, const ControlParams& params = {})
    {
        float frameHeight = ImGui::GetFrameHeight();
        ImVec2 buttonSize{frameHeight, frameHeight};

        ImGui::TableNextColumn();
        {
            ImGui::PushStyleColor(ImGuiCol_Button, color);
            ImGui::PushStyleVarX(ImGuiStyleVar_ButtonTextAlign, params.textAlignX);

            if(ImGui::Button(label, buttonSize))
                value = params.resetValue;

            ImGui::PopStyleVar();
            ImGui::PopStyleColor();

            ImGui::SameLine(0.f, 0.f);

            ImGui::PushID(label);
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::DragFloat("##", &value, params.speed, params.min, params.max, "%.2f", params.flags);
            ImGui::PopID();
        }
    }

    // For drawing X & Y controls with same params. In case they need to differ, there's drawControl()
    // It was meant to avoid constructing the params struct twice, but I should not be too worried with this
    // A better approach to avoid even more unnecessary constructions/deletions would be to create for
    // each occasion a local ControlParam inside the draw(component) function and pass it around
    /*static void drawVec2Control(float& x, float& y, const ImVec2& buttonSize, int32& id, const ControlParams& params = {})
    {
        drawControl("X", x, RED, buttonSize, id, params);
        drawControl("Y", y, GREEN, buttonSize, id, params);
    }*/

    static void drawDisabledControl(const char* label, const ImVec4& color, float textAlignX = 0.58f)
    {
        float frameHeight = ImGui::GetFrameHeight();
        ImVec2 buttonSize{frameHeight, frameHeight};

        ImGui::TableNextColumn();
        {
            ImGui::BeginDisabled();

            ImGui::PushStyleColor(ImGuiCol_Button, color);
            ImGui::PushStyleVarX(ImGuiStyleVar_ButtonTextAlign, textAlignX);

            ImGui::Button(label, buttonSize);

            ImGui::PopStyleVar();
            ImGui::PopStyleColor();

            ImGui::SameLine(0.f, 0.f);

            float dummy = 0.f;
            ImGui::PushID(label);
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::DragFloat("##", &dummy, 0.f, 0.f, 0.f, "%.2f");
            ImGui::PopID();

            ImGui::EndDisabled();
        }
    }

    static void beginRow(const char* label)
    {
        ImGui::PushID(label);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        {
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted(label);
        }
    }

    static void endRow()
    {
        ImGui::PopID(); // Label
    }

    // Meant to be used when the component table has more than 2 columns, as we can't easily merge columns in ImGui
    static void drawColorEdit(glm::vec4& color, float firstColumnWidth)
    {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().ItemSpacing.y);

        if(ImGui::BeginTable("Color", 2, ImGuiTableFlags_NoPadInnerX))
        {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, firstColumnWidth);

            beginRow("Color");
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::ColorEdit4("##c", glm::value_ptr(color));
            endRow();

            ImGui::EndTable();
        }
    }

    void draw(TransformComponent& transform, float firstColumnWidth)
    {
        if(ImGui::BeginTable("Transform", 4, ImGuiTableFlags_NoPadInnerX))
        {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, firstColumnWidth);

            beginRow("Translation");
                drawControl("X", transform.position.x, RED);
                drawControl("Y", transform.position.y, GREEN);
                drawControl("Z", transform.position.z, BLUE, {.max = 1.f, .speed = 0.01f, .flags = ImGuiSliderFlags_AlwaysClamp});
            endRow();

            beginRow("Scale");
                drawControl("X", transform.scale.x, RED, {.max = FLT_MAX, .flags = ImGuiSliderFlags_AlwaysClamp, .resetValue = 1.f});
                drawControl("Y", transform.scale.y, GREEN, {.max = FLT_MAX, .flags = ImGuiSliderFlags_AlwaysClamp, .resetValue = 1.f});
                drawDisabledControl("Z", BLUE);
            endRow();

            beginRow("Rotation");
                drawDisabledControl("X", RED);
                drawDisabledControl("Y", GREEN);
                drawControl("Z", transform.rotation, BLUE);
            endRow();

            ImGui::EndTable();
        }
    }

    void draw(CameraComponent& camera, float firstColumnWidth)
    {
        if(ImGui::BeginTable("Camera", 4, ImGuiTableFlags_NoPadInnerX))
        {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, firstColumnWidth);

            beginRow("Offset");
                drawControl("X", camera.camera.offset.x, RED);
                drawControl("Y", camera.camera.offset.y, GREEN);
                drawDisabledControl("Z", BLUE);
            endRow();

            beginRow("Zoom");
                drawControl("##z", camera.camera.zoom, GRAY, {.min = 0.1f, .max = FLT_MAX, .flags = ImGuiSliderFlags_AlwaysClamp, .resetValue = 1.f});
            endRow();

            ImGui::EndTable();
        }
    }

    void draw(SpriteComponent& sprite, float firstColumnWidth)
    {
        float frameHeight = ImGui::GetFrameHeight();

        if(ImGui::BeginTable("Sprite", 2, ImGuiTableFlags_NoPadInnerX))
        {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, firstColumnWidth);

            beginRow("Sample Area");
                ImGui::SetItemTooltip("x, y, width, height");
                ImGui::TableNextColumn();

                ImGui::PushStyleColor(ImGuiCol_Button, GRAY);
                if(ImGui::Button("##s", {frameHeight, frameHeight}))
                    sprite.texSampleArea = {};
                ImGui::PopStyleColor();

                ImGui::SameLine(0.f, 0.f);

                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::DragFloat4("##d", &sprite.texSampleArea.x, 0.1f, 0.f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            endRow();

            beginRow("Filter");
                ImGui::TableNextColumn();

                static int32 currentItem = (int32)sprite.textureFilter;

                ImGui::SetNextItemWidth(-FLT_MIN);
                if(ImGui::Combo("Filter", &currentItem, "Linear\0Nearest\0\0"))
                    sprite.texture->applyFilter(TextureFilter{currentItem});
            endRow();

            beginRow("Tint Color");
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::ColorEdit4("##c", glm::value_ptr(sprite.tintColor));
            endRow();

            ImGui::EndTable();
        }
    }

    void draw(RectangleComponent& rectangle, float firstColumnWidth)
    {
        if(ImGui::BeginTable("Rectangle", 4, ImGuiTableFlags_NoPadInnerX))
        {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, firstColumnWidth);

            beginRow("Size");
                drawControl("X", rectangle.size.x, RED, {.max = FLT_MAX, .flags = ImGuiSliderFlags_AlwaysClamp, .resetValue = 200.f});
                drawControl("Y", rectangle.size.y, GREEN, {.max = FLT_MAX, .flags = ImGuiSliderFlags_AlwaysClamp, .resetValue = 200.f});
                drawDisabledControl("Z", BLUE);
            endRow();

            ImGui::EndTable();
        }
        
        drawColorEdit(rectangle.color, firstColumnWidth);
    }

    void draw(CircleComponent& circle, float firstColumnWidth)
    {
        if(ImGui::BeginTable("Circle", 2, ImGuiTableFlags_NoPadInnerX))
        {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, firstColumnWidth);

            beginRow("Radius");
                drawControl("##r", circle.radius, GRAY, {.max = FLT_MAX, .flags = ImGuiSliderFlags_AlwaysClamp, .resetValue = 100.f});
            endRow();
            
            beginRow("Fill");
                drawControl("##f", circle.fillFactor, GRAY, {.max = 1.f, .speed = 0.01, .flags = ImGuiSliderFlags_AlwaysClamp, .resetValue = 1.f});
            endRow();

            beginRow("Fade");
                drawControl("##fa", circle.fadeFactor, GRAY, {.max = 4.f, .speed = 0.01, .flags = ImGuiSliderFlags_AlwaysClamp, .resetValue = 0.007f});
            endRow();

            beginRow("Color");
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::ColorEdit4("##c", glm::value_ptr(circle.color));
            endRow();

            ImGui::EndTable();
        }
    }

    template<unsigned N>
    bool inputTextMultiline(const char* label, std::u32string& source, ImGuiInputTextFlags flags = 0)
    {
        std::string temp = u32stringToString(source);

        // No need to initialize the buffer here, as it's going to only be used in this 
        char buffer[N + 1]; // function (std::string ctor taking a char* stops at the 1st '\0')
        uint32 numBytes = temp.size();

        if(numBytes > N)
            numBytes = N;

        std::memcpy(buffer, temp.c_str(), numBytes);
        buffer[numBytes] = '\0';

        ImGui::PushID(&source);

        bool modified = ImGui::InputTextMultiline(label, buffer, sizeof(buffer), {}, flags);

        ImGui::PopID();

        if(modified)
            source = stringToU32string(buffer);

        return modified;
    }

    void draw(TextComponent& text, float firstColumnWidth)
    {
        ImGui::SetNextItemWidth(-FLT_MIN);
        inputTextMultiline<2048>("##t", text.text);

        if(ImGui::BeginTable("Text", 2, ImGuiTableFlags_NoPadInnerX))
        {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, firstColumnWidth);

            beginRow("Font Size");
                drawControl("##f", text.fontSize, GRAY, {.resetValue = 40.f});
            endRow();

            beginRow("Kerning");
                drawControl("##k", text.kerning, GRAY, {.speed = 0.01f});
            endRow();

            beginRow("Line Spacing");
                drawControl("##l", text.lineSpacing, GRAY, {.speed = 0.01f});
            endRow();

            beginRow("Color");
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::ColorEdit4("##c", glm::value_ptr(text.color));
            endRow();

            ImGui::EndTable();
        }
    }
} // namespace mirras
