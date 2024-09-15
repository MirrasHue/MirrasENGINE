#pragma once

#include <vector>
#include <memory>

#include "Core/Layer.h"
#include "Core/BasicTypes.h"

namespace mirras
{
    class AppLayers
    {
    public:
        void addLayer(single_ref<Layer> layer);
        void addOverlay(single_ref<Layer> layer);

        auto begin() { return layers.begin(); }
        auto end()   { return layers.end(); }

        ~AppLayers()
        {
            for(auto& layer : layers)
                layer->unload();
        }

    private:
        std::vector<single_ref<Layer>> layers;
        uint32 layerInsertionIdx{};
    };
}