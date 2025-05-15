#include "Core/AppLayers.h"

#include "Core/Asserts.h"

namespace mirras
{
    void AppLayers::addLayer(single_ref<Layer> layer)
    {
        MIRR_ASSERT_CORE_RETURN(layer, "A null layer cannot be added");

        layer->load();
        layers.emplace(layers.begin() + layerInsertionIdx, std::move(layer));
        ++layerInsertionIdx;
    }

    void AppLayers::addOverlay(single_ref<Layer> layer)
    {
        MIRR_ASSERT_CORE_RETURN(layer, "A null layer cannot be added");
        
        layer->load();
        layers.emplace_back(std::move(layer));
    }
} // namespace mirras
