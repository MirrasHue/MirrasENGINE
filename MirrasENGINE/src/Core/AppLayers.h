#pragma once

#include <vector>
#include <memory>

#include "Core/Layer.h"
#include "Core/Utils.h"

namespace mirras
{
    class AppLayers
    {
    public:
        void addLayer(std::unique_ptr<Layer> layer);
        void addOverlay(std::unique_ptr<Layer> layer);

        auto begin() { return layers.begin(); }
        auto end()   { return layers.end(); }

        ~AppLayers()
        {
            for(auto& layer : layers)
                layer->unload();
        }

    private:
        std::vector<std::unique_ptr<Layer>> layers;
        uint32 layerInsertionIdx{};
    };
}