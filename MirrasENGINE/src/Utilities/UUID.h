#pragma once

#include "Core/Types/Integer.h"

namespace mirras
{
    class UUID
    {
    public:
        UUID();
        UUID(uint64 uuid) :
            id{uuid} {}

    private:
        uint64 id{};
    };
    
} // namespace mirras
