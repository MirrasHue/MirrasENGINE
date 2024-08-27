#pragma once

#include "Events/Event.h"
#include "Input/DeviceCodes.h"

namespace mirras
{
    class MouseButtonEvent : public Event
    {
    public:
        using MouseButton = Mouse;

        MouseButtonEvent() = default;
        MouseButtonEvent(MouseButton buttonCode, ModifierKeyFlag modifiers) :
            button{buttonCode}, keyModifierFlags{modifiers} {}

        ModifierKeyFlag keyModifierFlags{};
        MouseButton button = MouseButton::None;
    };

    class MouseButtonPressed : public MouseButtonEvent
    {
    public:
        Event_Runtime_Identification(EventType::MouseButtonPressed, EventCategory::Mouse)

        MouseButtonPressed() = default;
        MouseButtonPressed(MouseButton buttonCode, ModifierKeyFlag modifiers) :
            MouseButtonEvent{buttonCode, modifiers} {}
    };

    class MouseButtonReleased : public MouseButtonEvent
    {
    public:
        Event_Runtime_Identification(EventType::MouseButtonReleased, EventCategory::Mouse)

        MouseButtonReleased() = default;
        MouseButtonReleased(MouseButton buttonCode, ModifierKeyFlag modifiers) :
            MouseButtonEvent{buttonCode, modifiers} {}
    };

    class MouseMoved : public Event
    {
    public:
        Event_Runtime_Identification(EventType::MouseMoved, EventCategory::Mouse)

        MouseMoved() = default;
        MouseMoved(float x, float y) :
            mousePos{x, y} {}

        vec2f mousePos;
    };

    class MouseWheelScrolled : public Event
    {
    public:
        Event_Runtime_Identification(EventType::MouseWheelScrolled, EventCategory::Mouse)

        MouseWheelScrolled() = default;
        MouseWheelScrolled(float x, float y) :
            mouseWheelOffset{x, y} {}

        vec2f mouseWheelOffset;
    };
}