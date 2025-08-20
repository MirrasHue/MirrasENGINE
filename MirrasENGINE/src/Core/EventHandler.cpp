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
            front.sink<WindowResized>().connect<&App::onAppEvent>(app);
            front.sink<WindowClosed>().connect<&App::onAppEvent>(app);
            front.sink<MouseMoved>().connect<&App::onAppEvent>(app);
            front.sink<MouseButtonPressed>().connect<&App::onAppEvent>(app);
            front.sink<MouseButtonReleased>().connect<&App::onAppEvent>(app);
            front.sink<MouseWheelScrolled>().connect<&App::onAppEvent>(app);
            front.sink<KeyPressed>().connect<&App::onAppEvent>(app);
            front.sink<KeyReleased>().connect<&App::onAppEvent>(app);
            front.sink<TextEntered>().connect<&App::onAppEvent>(app);

            back.sink<WindowResized>().connect<&App::onAppEvent>(app);
            back.sink<WindowClosed>().connect<&App::onAppEvent>(app);
            back.sink<MouseMoved>().connect<&App::onAppEvent>(app);
            back.sink<MouseButtonPressed>().connect<&App::onAppEvent>(app);
            back.sink<MouseButtonReleased>().connect<&App::onAppEvent>(app);
            back.sink<MouseWheelScrolled>().connect<&App::onAppEvent>(app);
            back.sink<KeyPressed>().connect<&App::onAppEvent>(app);
            back.sink<KeyReleased>().connect<&App::onAppEvent>(app);
            back.sink<TextEntered>().connect<&App::onAppEvent>(app);
        )

        NO_ASYNC_UPDATE (
            onEventCallback.connect<&App::onAppEvent>(app);
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
