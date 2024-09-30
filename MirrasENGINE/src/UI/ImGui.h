#pragma once

#include "Core/Fwd.h"

namespace mirras::imgui
{
    void init();
    void beginFrame();
    void endFrame();
    void onEvent(Event& event);
    void shutdown();
} // namespace mirras::imgui
