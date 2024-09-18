#pragma once

#include "Events/Event.h"

namespace mirras::imgui
{
    void init();
    void beginFrame();
    void endFrame();
    void onEvent(Event& event);
    void shutdown();
}