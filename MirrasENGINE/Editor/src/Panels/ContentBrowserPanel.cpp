#include "Panels/ContentBrowserPanel.h"

#include "Utilities/Encodings.h"

#include <imgui/imgui.h>

namespace mirras
{
    ContentBrowserPanel::ContentBrowserPanel() :
        baseDirectory{"Assets"}, currentDirectory{"Assets"}
    {
        directoryIcon = Texture::loadFrom("Assets/Icons/directory.png");
        fileIcon = Texture::loadFrom("Assets/Icons/file.png");
    }

    void ContentBrowserPanel::draw()
    {
        ImGui::Begin("Content Browser", nullptr, ImGuiWindowFlags_NoCollapse);

        float frameHeight = ImGui::GetFrameHeight();

        if(currentDirectory != baseDirectory)
        {
            if(ImGui::ArrowButton("##a", ImGuiDir_Left))
                currentDirectory = currentDirectory.parent_path();
        }
        else
            ImGui::InvisibleButton("##i", {frameHeight, frameHeight});

        float framePaddingX = ImGui::GetStyle().FramePadding.x * 2.f;
        float columnWidth = iconSize + spacingX + framePaddingX;
        float width = ImGui::GetContentRegionAvail().x;
        int32 columns = width / columnWidth;

        // Check if all columns won't actually fit
        if(columns * (columnWidth + framePaddingX) > width)
            --columns;

        // Check if we can fit one more icon (not with full column width)
        if(columns * (columnWidth + framePaddingX) <= width - iconSize)
            ++columns;

        columns = std::max(columns, 1);

        if(ImGui::BeginTable("Content Browser", columns, ImGuiTableFlags_NoClip))
        {
            // Setup columns (need to be done before submitting any column)
            for(int32 i = 0; i < columns; ++i)
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, columnWidth);

            for(const auto& pathEntry : std::filesystem::directory_iterator{currentDirectory})
            {
                ImGui::TableNextColumn();

                // path.string() on MSVC doesn't convert UTF-16 to UTF-8, so we have to explicitly call u8string()
                const auto& path = pathEntry.path();
                auto filename = utf::toString(path.filename().u8string());
                const Texture& icon = pathEntry.is_directory() ? *directoryIcon : *fileIcon;

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
                // TODO: Use a InvisibleButton or Selectable to cover both the icon and filename, so we can drag from the text as well
                ImGui::ImageButton(filename.c_str(), icon.id, {iconSize, iconSize});
                ImGui::PopStyleColor();

                // For now we don't need to use directories as drag and drop sources
                if(!pathEntry.is_directory() && ImGui::BeginDragDropSource())
                {
                    auto filePath = path.u8string();

                    ImGui::Image(icon.id, {frameHeight, frameHeight});
                    ImGui::SameLine();
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted(filename.c_str());
                    ImGui::SetDragDropPayload("CONTENT_BROWSER_FILE", filePath.c_str(), filePath.size() + 1);
                    ImGui::EndDragDropSource();
                }

                if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    if(pathEntry.is_directory())
                        currentDirectory /= path.filename();
                }

                // TODO: clip the text if it spans over > 2 lines and show it on a tooltip when hovered
                ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + iconSize + framePaddingX * 1.5f);
                ImGui::TextUnformatted(filename.c_str());
                ImGui::PopTextWrapPos();
            }

            ImGui::EndTable();
        }

        if(ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsWindowHovered())
        {
            float mouseWheel = ImGui::GetIO().MouseWheel;

            if(ImGui::IsKeyDown(ImGuiMod_Shift))
            {
                spacingX += mouseWheel;
                spacingX = std::max(spacingX, 0.f);
            }
            else
            {
                iconSize += mouseWheel;
                iconSize = std::max(iconSize, 64.f);
            }
        }

        ImGui::End();
    }
} // namespace mirras
