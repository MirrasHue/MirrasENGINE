#include "Widgets/Entity.h"

#include <glm/gtc/type_ptr.hpp>

static constexpr ImVec4 RED{1.f, 0.f, 0.f, 1.f};
static constexpr ImVec4 GREEN{0.f, 0.9f, 0.f, 1.f};
static constexpr ImVec4 BLUE{0.f, 0.f, 1.f, 1.f};

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

    static void drawControl(const char* label, float& value, const ImVec4& color, const ImVec2& buttonSize, const ControlParams& params = {})
    {
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

    static void drawDisabledControl(const char* label, const ImVec4& color, const ImVec2& buttonSize, float textAlignX = 0.58f)
    {
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

    void draw(TransformComponent& transform, float firstColumnWidth)
    {
        if(ImGui::BeginTable("Transform", 4, ImGuiTableFlags_NoPadInnerX))
        {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, firstColumnWidth);
            
            float frameHeight = ImGui::GetFrameHeight();
            ImVec2 buttonSize{frameHeight, frameHeight};
 
            beginRow("Translation");
            {
                drawControl("X", transform.position.x, RED, buttonSize);
                drawControl("Y", transform.position.y, GREEN, buttonSize);
                drawControl("Z", transform.position.z, BLUE, buttonSize, {.max = 1.f, .speed = 0.01f, .flags = ImGuiSliderFlags_AlwaysClamp});
            }
            endRow();

            beginRow("Scale");
            {
                drawControl("X", transform.scale.x, RED, buttonSize, {.max = FLT_MAX, .flags = ImGuiSliderFlags_AlwaysClamp, .resetValue = 1.f});
                drawControl("Y", transform.scale.y, GREEN, buttonSize, {.max = FLT_MAX, .flags = ImGuiSliderFlags_AlwaysClamp, .resetValue = 1.f});
                drawDisabledControl("Z", BLUE, buttonSize);
            }
            endRow();

            beginRow("Rotation");
            {
                drawDisabledControl("X", RED, buttonSize);
                drawDisabledControl("Y", GREEN, buttonSize);
                drawControl("Z", transform.rotation, BLUE, buttonSize);
            }
            endRow();

            ImGui::EndTable();
        }
    }

    void draw(CameraComponent& camera, float firstColumnWidth)
    {
        ImGui::Text("Camera");
    }

    void draw(SpriteComponent& sprite, float firstColumnWidth)
    {
        ImGui::Text("Sprite");
    }

    void draw(RectangleComponent& rectangle, float firstColumnWidth)
    {
        if(ImGui::BeginTable("Rectangle", 4, ImGuiTableFlags_NoPadInnerX))
        {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, firstColumnWidth);

            float frameHeight = ImGui::GetFrameHeight();
            ImVec2 buttonSize{frameHeight, frameHeight};

            beginRow("Size");
            {
                drawControl("X", rectangle.size.x, RED, buttonSize, {.max = FLT_MAX, .flags = ImGuiSliderFlags_AlwaysClamp, .resetValue = 200.f});
                drawControl("Y", rectangle.size.y, GREEN, buttonSize, {.max = FLT_MAX, .flags = ImGuiSliderFlags_AlwaysClamp, .resetValue = 200.f});
                drawDisabledControl("Z", BLUE, buttonSize);
            }
            endRow();

            ImGui::EndTable();
        }

        ImGui::ColorEdit4("Color", glm::value_ptr(rectangle.color));
    }

    void draw(CircleComponent& circle, float firstColumnWidth)
    {
        ImGui::Text("Circle");
    }

    void draw(TextComponent& text, float firstColumnWidth)
    {
        ImGui::Text("Text");
    }
} // namespace mirras
