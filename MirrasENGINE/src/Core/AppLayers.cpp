#include "Core/AppLayers.h"
#include "Core/Asserts.h"

namespace mirras
{
    void AppLayers::addLayer(std::unique_ptr<Layer> layer)
    {
        MIRR_ASSERT_CORE(layer, "Can't add a null layer");

        layer->load();
        layers.emplace(layers.begin() + layerInsertionIdx, std::move(layer));
        ++layerInsertionIdx;
    }

    void AppLayers::addOverlay(std::unique_ptr<Layer> layer)
    {
        MIRR_ASSERT_CORE(layer, "Can't add a null layer");
        
        layer->load();
        layers.emplace_back(std::move(layer));
    }
}