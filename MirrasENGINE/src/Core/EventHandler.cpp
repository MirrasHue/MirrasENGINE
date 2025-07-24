#include "Core/EventHandler.h"

#include "Core/Application.h"

#include "Events/MouseEvents.h"
#include "Events/KeyboardEvents.h"
#include "Events/WindowEvents.h"

namespace mirras
{
    void EventHandler::connectEvents(App* app)
    {
        onResizeCallback.connect<&App::onWindowResize>(app);

        ASYNC_UPDATE
        (
            front.sink<WindowResized>().connect<&App::onEvent>(app);
            front.sink<WindowClosed>().connect<&App::onEvent>(app);
            front.sink<MouseMoved>().connect<&App::onEvent>(app);
            front.sink<MouseButtonPressed>().connect<&App::onEvent>(app);
            front.sink<MouseButtonReleased>().connect<&App::onEvent>(app);
            front.sink<MouseWheelScrolled>().connect<&App::onEvent>(app);
            front.sink<KeyPressed>().connect<&App::onEvent>(app);
            front.sink<KeyReleased>().connect<&App::onEvent>(app);
            front.sink<TextEntered>().connect<&App::onEvent>(app);

            back.sink<WindowResized>().connect<&App::onEvent>(app);
            back.sink<WindowClosed>().connect<&App::onEvent>(app);
            back.sink<MouseMoved>().connect<&App::onEvent>(app);
            back.sink<MouseButtonPressed>().connect<&App::onEvent>(app);
            back.sink<MouseButtonReleased>().connect<&App::onEvent>(app);
            back.sink<MouseWheelScrolled>().connect<&App::onEvent>(app);
            back.sink<KeyPressed>().connect<&App::onEvent>(app);
            back.sink<KeyReleased>().connect<&App::onEvent>(app);
            back.sink<TextEntered>().connect<&App::onEvent>(app);
        )

        NO_ASYNC_UPDATE (
            onEventCallback.connect<&App::onEvent>(app);
        )
    }

    void EventHandler::process()
    {
        ASYNC_UPDATE
        (
            {
                std::lock_guard lock{swapMutex};
                front.swap(back);
            }

            front.update();
        )
    }

    void EventHandler::handle(MouseMoved&& event)
    {
        ASYNC_UPDATE
        (
            std::lock_guard lock{swapMutex};

            back.clear<MouseMoved>(); // Keep only the latest mouse move
            back.enqueue(event);
        )

        NO_ASYNC_UPDATE (
            onEventCallback(event);
        )
    }

    void EventHandler::dispatchResize(WindowResized& event)
    {
        onResizeCallback(event);

        ASYNC_UPDATE (
            handle(event); // Just in case someone wants to be notified about resizes
        )

        NO_ASYNC_UPDATE (
            onEventCallback(event);
        )
    }
} // namespace mirras
