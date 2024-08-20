#pragma once

#include "Events/Event.h"
#include "Core/Utils.h"

namespace mirras
{
    class WindowResized : public Event
    {
    public:
        Event_Runtime_Identification(EventType::WindowResized)

        WindowResized() = default;
        WindowResized(int32 w, int32 h) :
            windowSize{w, h} {}

        vec2i windowSize;
    };

    class WindowMoved : public Event
    {
    public:
        Event_Runtime_Identification(EventType::WindowMoved)

        WindowMoved() = default;
        WindowMoved(int32 x, int32 y) :
            windowPos{x, y} {}

        vec2i windowPos;
    };

    class WindowClosed : public Event
    {
    public:
        Event_Runtime_Identification(EventType::WindowClosed)
    };

    class WindowFocusGained : public Event
    {
    public:
        Event_Runtime_Identification(EventType::WindowFocusGained)
    };

    class WindowFocusLost : public Event
    {
    public:
        Event_Runtime_Identification(EventType::WindowFocusLost)
    };
}