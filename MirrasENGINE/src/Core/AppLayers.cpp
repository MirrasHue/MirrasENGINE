#include "Core/AppLayers.h"

#include "Core/Asserts.h"

namespace mirras
{
    void AppLayers::addLayer(single_ref<Layer> layer)
    {
        MIRR_ASSERT_CORE_RETURN(layer, "A null layer cannot be added");

        auto it = layers.emplace(layers.begin() + layerInsertionIdx, std::move(layer));
        ++layerInsertionIdx;

        // Loading after the insertion allows for correctly adding other layers from inside load(),
        // which might be useful if some extra logic is required to determine whether add them or not
        (*it)->load();
    }

    void AppLayers::addOverlay(single_ref<Layer> layer)
    {
        MIRR_ASSERT_CORE_RETURN(layer, "A null layer cannot be added");
        
        auto& ref = layers.emplace_back(std::move(layer));
        ref->load();
    }
} // namespace mirras
