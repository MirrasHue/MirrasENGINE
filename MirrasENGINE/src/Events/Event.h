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
        template<typename EventType>
        static bool is_a(Event& event); // This doesn't take the parent class into consideration

        template<typename EventType, auto func>
        static bool dispatch(Event& event);

        template<typename EventType, auto memberFunc, typename InstanceType>
        static bool dispatch_to_member(Event& event, InstanceType* this_ptr);

        virtual EventType getEventType() const = 0;
        virtual EventCategory getEventCategory() const = 0;

        bool isInCategory(EventCategory category) { return getEventCategory() == category; }

        virtual ~Event() = default;

        bool propagable = true;
    };

    template <typename EventType>
    bool Event::is_a(Event& event)
    {
        if(event.getEventType() == EventType::getEventTypeStatic())
            return true;

        return false;
    }

    template <typename EventType, auto func>
    bool Event::dispatch(Event& event)
    {
        if(Event::is_a<EventType>(event))
        {
            func(static_cast<EventType&>(event));

            return true;
        }

        return false;
    }

    template <typename EventType, auto memberFunc, typename InstanceType>
    bool Event::dispatch_to_member(Event& event, InstanceType* this_ptr)
    {
        if(!this_ptr)
            return false;
            
        if(Event::is_a<EventType>(event))
        {
            std::invoke(memberFunc, this_ptr, static_cast<EventType&>(event));

            return true;
        }

        return false;
    }

    #define Event_Runtime_Identification(event_type, event_category)  \
        virtual EventType getEventType() const { return event_type; } \
        static EventType getEventTypeStatic()  { return event_type; } \
        virtual EventCategory getEventCategory() const { return event_category; }
} // namespace mirras
