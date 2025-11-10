#pragma once

#include "Core/Types/Basic.h"
#include "Utilities/Encodings.h"

#include <string>
#include <cstring>

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

    uint8 isItemSingleOrDoubleClicked(ImGuiMouseButton button);
} // namespace mirras
