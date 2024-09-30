#pragma once

#include "Events/Event.h"

#include "Input/DeviceCodes.h"

namespace mirras
{
    class KeyEvent : public Event
    {
    public:
        using KeyCode = Key;
        
        KeyEvent() = default;
        KeyEvent(KeyCode code, int32 scan, ModifierKeyFlag modifiers) :
            key{code}, scanCode{scan}, keyModifierFlags{modifiers} {}

        KeyCode key{};
        int32 scanCode{};
        ModifierKeyFlag keyModifierFlags{};
    };

    class KeyPressed : public KeyEvent
    {
    public:
        Event_Runtime_Identification(EventType::KeyPressed, EventCategory::Keyboard)

        KeyPressed() = default;
        KeyPressed(KeyCode code, int32 scan, ModifierKeyFlag modifiers, bool isRepeat = false) :
            KeyEvent{code, scan, modifiers}, isRepeated{isRepeat} {}

        bool isRepeated{};
    };

    class KeyReleased : public KeyEvent
    {
    public:
        Event_Runtime_Identification(EventType::KeyReleased, EventCategory::Keyboard)

        KeyReleased() = default;
        KeyReleased(KeyCode code, int32 scan, ModifierKeyFlag modifiers) :
            KeyEvent{code, scan, modifiers} {}
    };

    class TextEntered : public Event
    {
    public:
        Event_Runtime_Identification(EventType::TextEntered, EventCategory::Keyboard)

        TextEntered() = default;
        TextEntered(uint32 code) :
            unicode{code} {}

        uint32 unicode{};
    };
} // namespace mirras
