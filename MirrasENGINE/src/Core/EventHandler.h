#pragma once

#include "Core/AsyncMacros.h"
#include "Core/Fwd.h"

#include <entt/signal/delegate.hpp>

#ifdef RUN_UPDATE_THREAD
    #include <mutex>
    #include <entt/signal/dispatcher.hpp>
#endif

namespace mirras
{
    class App;

    class EventHandler
    {
    public:
        EventHandler() = default;

        void connectEvents(App* app);

        // Process all queued events (only for async update)
        void process();

        template<typename Type>
        void handle(Type&& event)
        {
            ASYNC_UPDATE
            (
                std::lock_guard lock{swapMutex};
                back.enqueue(event);
            )

            NO_ASYNC_UPDATE (
                onEventCallback(event);
            )
        }

        void handle(MouseMoved&& event);

        void dispatchResize(WindowResized& event);

    private:
        ASYNC_UPDATE
        (
            std::mutex swapMutex;
            entt::dispatcher front;
            entt::dispatcher back;
        )

        NO_ASYNC_UPDATE (
            entt::delegate<void(Event&)> onEventCallback;
        )

        entt::delegate<void(WindowResized&)> onResizeCallback;
    };
} // namespace mirras
