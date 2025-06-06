#pragma once

#include <functional>

namespace mirras
{
    enum class EventType
	{
		WindowResized, WindowMoved, WindowClosed, WindowFocusGained, WindowFocusLost,
		KeyPressed, KeyReleased, TextEntered,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseWheelScrolled
	};

    enum class EventCategory
    {
        Window, 
        Keyboard,
        Mouse
    };

    class Event
    {
    public:
        template<typename EventClass>
        static bool is_a(Event& event); // This doesn't take the parent class into consideration

        template<typename EventClass, auto func>
        static bool dispatch(Event& event);

        template<typename EventClass, auto memberFunc, typename Object>
        static bool dispatch_to_member(Event& event, Object* this_pointer);

        virtual EventType getEventType() const = 0;
        virtual EventCategory getEventCategory() const = 0;

        bool isInCategory(EventCategory category) { return getEventCategory() == category; }

        virtual ~Event() = default;

        bool propagable = true;
    };

    template <typename EventClass>
    bool Event::is_a(Event& event)
    {
        if(event.getEventType() == EventClass::getEventTypeStatic())
            return true;

        return false;
    }

    template <typename EventClass, auto func>
    bool Event::dispatch(Event& event)
    {
        if(Event::is_a<EventClass>(event))
        {
            func(static_cast<EventClass&>(event));

            return true;
        }

        return false;
    }

    template <typename EventClass, auto memberFunc, typename Object>
    bool Event::dispatch_to_member(Event& event, Object* this_pointer)
    {
        if(!this_pointer)
            return false;
            
        if(Event::is_a<EventClass>(event))
        {
            std::invoke(memberFunc, this_pointer, static_cast<EventClass&>(event));

            return true;
        }

        return false;
    }

    #define Event_Runtime_Identification(event_type, event_category)  \
        virtual EventType getEventType() const { return event_type; } \
        static EventType getEventTypeStatic()  { return event_type; } \
        virtual EventCategory getEventCategory() const { return event_category; }
} // namespace mirras
