#pragma once

#include <unordered_map>

#include <imgui/imgui.h>

namespace mirras
{
    // Unfortunately ImGui doesn't clamp pasted text that exceeds the buffer size (it is completely
    // ignored, instead of pasting what could still fit). It seems to be a limitation of stb_textedit
    template<unsigned N>
    inline bool inputText(const char* label, std::string& source, ImGuiInputTextFlags flags = 0)
    {
        // No need to initialize the buffer here, as it's going to only be used in this 
        char buffer[N + 1]; // function (std::string ctor taking a char* stops at the 1st '\0')
        uint32 numBytes = source.size();

        if(numBytes > N)
            numBytes = N;

        std::memcpy(buffer, source.c_str(), numBytes);
        buffer[numBytes] = '\0';

        ImGui::PushID(&source);

        bool modified = ImGui::InputText(label, buffer, sizeof(buffer), flags);

        ImGui::PopID();

        if(modified)
            source = std::string{buffer};

        return modified;
    }

    template<unsigned N>
    inline bool inputTextMultiline(const char* label, std::u32string& source, ImVec2 size = {}, ImGuiInputTextFlags flags = 0)
    {
        std::string temp = utf::toString(source);

        char buffer[N + 1];
        uint32 numBytes = temp.size();

        if(numBytes > N)
            numBytes = N;

        std::memcpy(buffer, temp.c_str(), numBytes);
        buffer[numBytes] = '\0';

        ImGui::PushID(&source);

        bool modified = ImGui::InputTextMultiline(label, buffer, sizeof(buffer), size, flags | ImGuiInputTextFlags_AllowTabInput);

        ImGui::PopID();

        if(modified)
            source = utf::toU32string(buffer);

        return modified;
    }

    inline uint8 isItemSingleOrDoubleClicked(ImGuiMouseButton button)
    {
        // This workaround is meant to make single and double clicks on the same item behave
        // nicely (also avoids missing clicks when leaving the button area right after clicking)
        static std::unordered_map<ImGuiID, bool> singleClickMap;

        // It will insert a default {ID, false} pair the first time the ID is used
        bool& singleClick = singleClickMap[ImGui::GetItemID()];

        if(ImGui::IsItemHovered())
        {
            if(ImGui::IsMouseDoubleClicked(button))
            {
                singleClick = false;
                return 2;
            }
            else
            if(ImGui::IsMouseClicked(button))
                singleClick = true;
        }

        if(singleClick)
        {
            ImGuiIO& io = ImGui::GetIO();
            bool releasedWithDelay = ImGui::IsMouseReleasedWithDelay(button, io.MouseDoubleClickTime);

            if(releasedWithDelay && io.MouseClickedLastCount[button] == 1)
            {
                singleClick = false;
                return 1;
            }
        }

        return 0;
    }
} // namespace mirras
