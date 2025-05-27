#pragma once

#include "Core/Fwd.h"

namespace mirras::imgui
{
    void init();
    void beginFrame();
    void endFrame();
    // Allow the propagation of events even when ImGui wants to capture them
    void ignoreEventCapturing(bool ignore = true);
    void onEvent(Event& event);
    void shutdown();
} // namespace mirras::imgui
