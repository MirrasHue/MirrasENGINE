#include "Utilities/Encodings.h"

#include "Core/Log.h"

#include <msdf/utf8.h>

namespace mirras
{
    std::u32string stringToU32string(const std::string& str)
    {
        static std::vector<uint32> codepoints;
        // To avoid reallocating every time the function is called or the string size changes
        if(codepoints.capacity() < str.size())
            codepoints.reserve(str.size() * 2);
        else
        if(codepoints.capacity() > str.size() * 8)
            codepoints.shrink_to_fit();

        codepoints.clear();

        msdf_atlas::utf8Decode(codepoints, str.data());

        return std::u32string{codepoints.begin(), codepoints.end()};
    }

    std::string u32stringToString(const std::u32string& u32str)
    {
        static std::vector<char> str;

        if(str.capacity() < u32str.size() * 2)
            str.reserve(u32str.size() * 4);
        else
        if(str.capacity() > u32str.size() * 4 * 8)
            str.shrink_to_fit();

        str.clear();

        // Adapted from https://github.com/nemtrif/utfcpp/blob/master/source/utf8/core.h
        for(char32_t cp : u32str)
        {
            if(cp < 0x80) // 1 byte
                str.push_back(static_cast<char>(cp));
            else
            if(cp < 0x800) // 2 bytes
            {
                str.push_back(static_cast<char>(0xC0 | (cp >> 6)));
                str.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
            }
            else
            if(cp < 0x10000) // 3 bytes
            {
                str.push_back(static_cast<char>(0xE0 | (cp >> 12)));
                str.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
                str.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
            }
            else
            if(cp < 0x110000) // 4 bytes
            {
                str.push_back(static_cast<char>(0xF0 | (cp >> 18)));
                str.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
                str.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
                str.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
            }
            else
            {
                ENGINE_LOG_WARN("Invalid UTF-8 codepoint: {}. Stopping to parse", (uint32)cp);
                break;
            }
        }

        return std::string{str.begin(), str.end()};
    }
} // namespace mirras
