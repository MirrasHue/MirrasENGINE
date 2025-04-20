#include "Utilities/FileIO.h"

#include "Core/Log.h"
#include "Core/Types/Integer.h"

#include <fstream>

namespace mirras::file
{
    std::string read(const fs::path& filepath)
    {
        std::ifstream file{filepath, std::ios::binary};

        if(!file.is_open())
        {
            ENGINE_LOG_ERROR("Could not open file: {}", filepath.string());
            return {};
        }

        uint64 size = fs::file_size(filepath);

        std::string fileContent;
        fileContent.resize(size);

        if(!file.read(fileContent.data(), size))
        {
            ENGINE_LOG_ERROR("Failed to read file: {}", filepath.string());
            return {};
        }

        return fileContent;
    }
} // namespace mirras::file
