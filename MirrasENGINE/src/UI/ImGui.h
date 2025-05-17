#pragma once

#include "Core/Fwd.h"

namespace mirras::imgui
{
    void init();
    void beginFrame();
    void endFrame();
    void blockEvents(bool block = true);
    void onEvent(Event& event);
    void shutdown();
} // namespace mirras::imgui
