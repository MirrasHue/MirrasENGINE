#include "Utilities/UUID.h"
#include "Utilities/Random.h"

namespace mirras
{
    UUID::UUID() :
        id{Rand::next()} {}
} // namespace mirras
