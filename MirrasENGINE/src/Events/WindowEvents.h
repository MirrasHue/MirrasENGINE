#pragma once

#include "Events/Event.h"
#include "Core/BasicTypes.h"

namespace mirras
{
    class WindowResized : public Event
    {
    public:
        Event_Runtime_Identification(EventType::WindowResized, EventCategory::Window)

        WindowResized() = default;
        WindowResized(int32 w, int32 h) :
            windowSize{w, h} {}

        vec2i windowSize;
    };

    class WindowMoved : public Event
    {
    public:
        Event_Runtime_Identification(EventType::WindowMoved, EventCategory::Window)

        WindowMoved() = default;
        WindowMoved(int32 x, int32 y) :
            windowPos{x, y} {}

        vec2i windowPos;
    };

    class WindowClosed : public Event
    {
    public:
        Event_Runtime_Identification(EventType::WindowClosed, EventCategory::Window)
    };

    class WindowFocusGained : public Event
    {
    public:
        Event_Runtime_Identification(EventType::WindowFocusGained, EventCategory::Window)
    };

    class WindowFocusLost : public Event
    {
    public:
        Event_Runtime_Identification(EventType::WindowFocusLost, EventCategory::Window)
    };
}