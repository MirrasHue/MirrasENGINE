#pragma once

#include "Core/Types/Integer.h"

#include "Utilities/Random.h"

namespace mirras
{
    class UUID
    {
    public:
        UUID() :
            id{Rand::next()} {}

        UUID(uint64 uuid) :
            id{uuid} {}

        operator uint64() { return id; }

    private:
        uint64 id{};
    };
    
} // namespace mirras
