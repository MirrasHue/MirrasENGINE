#pragma once

#include "Core/Layer.h"
#include "Core/BasicTypes.h"

#include <vector>
#include <memory>

namespace mirras
{
    class AppLayers
    {
    public:
        void addLayer(single_ref<Layer> layer);
        void addOverlay(single_ref<Layer> layer);

        auto begin() const { return layers.begin(); }
        auto end()   const { return layers.end(); }

        ~AppLayers()
        {
            for(auto& layer : layers)
                layer->unload();
        }

    private:
        std::vector<single_ref<Layer>> layers;
        uint32 layerInsertionIdx{};
    };
} // namespace mirras
