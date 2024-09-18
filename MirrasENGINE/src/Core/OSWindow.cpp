#include "Core/OSWindow.h"
#include "Core/Log.h"
#include "Core/Asserts.h"
#include "Core/Utils.h"

#include "Events/WindowEvents.h"
#include "Events/KeyboardEvents.h"
#include "Events/MouseEvents.h"

namespace mirras
{
    static void glfwErrorCallback(int32 errorCode, const char* what);

    OSWindow::OSWindow(const WindowSpecs& windowSpecs)
    {
        glfwSetErrorCallback(glfwErrorCallback);

        if(!glfwInit())
            external_adversity("Could not initialize GLFW\n");

        /*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

        GLFWmonitor* monitor = nullptr;
        int32 width = windowSpecs.width;
        int32 height = windowSpecs.height;

        if(windowSpecs.fullScreen)
        {
            monitor = glfwGetPrimaryMonitor();
            auto videoMode = glfwGetVideoMode(monitor);
            width = videoMode->width;
            height = videoMode->height;
        }

        windowHandle = glfwCreateWindow(width, height, windowSpecs.title.data(), monitor, nullptr);

        // We don't use the size in the specs, because the created window can't always match the requested size
        initialFbSize = getFramebufferSize();

        // GLFW only applies the limits if both minimum/maximum values are different from -1
        glfwSetWindowSizeLimits(windowHandle, windowSpecs.minWidth, windowSpecs.minHeight, -1, -1); // -1 here indicates there's no limits

        if(windowSpecs.keepAspectRatio)
            glfwSetWindowAspectRatio(windowHandle, width, height);

        glfwMakeContextCurrent(windowHandle);

        glfwSwapInterval(windowSpecs.VSync);
        OSWindow::VSyncEnabled = windowSpecs.VSync;

        glfwSetWindowUserPointer(windowHandle, &appCallbacks);

        setGLFWCallbacks();
    }

    vec2i OSWindow::getFramebufferSize() const
    {
        vec2i size;
        glfwGetFramebufferSize(windowHandle, &size.x, &size.y);
        return size;
    }

    void OSWindow::setGLFWCallbacks()
    {
        glfwSetFramebufferSizeCallback(windowHandle, [](GLFWwindow* window, int32 width, int32 height)
        {
            auto appCallbacks = static_cast<AppCallbacks*>(glfwGetWindowUserPointer(window));

            MIRR_ASSERT_CORE(appCallbacks, "User pointer not set for the current window!");
            
            WindowResized event{width, height};

            appCallbacks->onEvent(event);
        });

        glfwSetWindowCloseCallback(windowHandle, [](GLFWwindow* window)
        {
            auto appCallbacks = static_cast<AppCallbacks*>(glfwGetWindowUserPointer(window));

            MIRR_ASSERT_CORE(appCallbacks, "User pointer not set for the current window!");

            WindowClosed event;

            appCallbacks->onEvent(event);
        });

        glfwSetKeyCallback(windowHandle, [](GLFWwindow* window, int32 keyCode, int32 scanCode, int32 state, int32 modifierFlags)
        {
            auto appCallbacks = static_cast<AppCallbacks*>(glfwGetWindowUserPointer(window));

            MIRR_ASSERT_CORE(appCallbacks, "User pointer not set for the current window!");

            switch(KeyState{state})
            {
            case KeyState::Pressed:
                {
                    KeyPressed event{Key{keyCode}, scanCode, ModifierKeyFlag{(uint8)modifierFlags}};
                    appCallbacks->onEvent(event);
                    break;
                }
            case KeyState::Released:
                {
                    KeyReleased event{Key{keyCode}, scanCode, ModifierKeyFlag{(uint8)modifierFlags}};
                    appCallbacks->onEvent(event);
                    break;
                }
            case KeyState::Repeated:
                {
                    KeyPressed event{Key{keyCode}, scanCode, ModifierKeyFlag{(uint8)modifierFlags}, true};
                    appCallbacks->onEvent(event);
                    break;
                }
            }
        });

        glfwSetCharCallback(windowHandle, [](GLFWwindow* window, uint32 unicode)
        {
            auto appCallbacks = static_cast<AppCallbacks*>(glfwGetWindowUserPointer(window));

            MIRR_ASSERT_CORE(appCallbacks, "User pointer not set for the current window!");

            TextEntered event{unicode};

            appCallbacks->onEvent(event);
        });

        glfwSetMouseButtonCallback(windowHandle, [](GLFWwindow* window, int32 button, int32 state, int32 modifierFlags)
        {
            auto appCallbacks = static_cast<AppCallbacks*>(glfwGetWindowUserPointer(window));

            MIRR_ASSERT_CORE(appCallbacks, "User pointer not set for the current window!");

            switch(MouseButtonState{state})
            {
            case MouseButtonState::Pressed:
                {
                    MouseButtonPressed event{Mouse{button}, ModifierKeyFlag{(uint8)modifierFlags}};
                    appCallbacks->onEvent(event);
                    break;
                }
            case MouseButtonState::Released:
                {
                    MouseButtonReleased event{Mouse{button}, ModifierKeyFlag{(uint8)modifierFlags}};
                    appCallbacks->onEvent(event);
                    break;
                }
            }
        });

        glfwSetCursorPosCallback(windowHandle, [](GLFWwindow* window, double xPos, double yPos)
        {
            auto appCallbacks = static_cast<AppCallbacks*>(glfwGetWindowUserPointer(window));

            MIRR_ASSERT_CORE(appCallbacks, "User pointer not set for the current window!");

            MouseMoved event{(float)xPos, (float)yPos};

            appCallbacks->onEvent(event);
        });

        glfwSetScrollCallback(windowHandle, [](GLFWwindow* window, double xOffset, double yOffset)
        {
            auto appCallbacks = static_cast<AppCallbacks*>(glfwGetWindowUserPointer(window));

            MIRR_ASSERT_CORE(appCallbacks, "User pointer not set for the current window!");

            MouseWheelScrolled event{(float)xOffset, (float)yOffset};

            appCallbacks->onEvent(event);
        });
    }

    class glfw_error : public std::runtime_error
    {
    public:
        glfw_error(const char* what) :
            std::runtime_error{what} {}
    };

    OSWindow::~OSWindow()
    {
        glfwDestroyWindow(windowHandle);
        std::atexit(glfwTerminate);
    }

    void glfwErrorCallback(int32 errorCode, const char* what)
    {
        // Application programmer errors, should be fixed by using GLFW properly
        MIRR_ASSERT_CORE(errorCode != GLFW_NOT_INITIALIZED, what);
        MIRR_ASSERT_CORE(errorCode != GLFW_NO_CURRENT_CONTEXT, what);
        MIRR_ASSERT_CORE(errorCode != GLFW_NO_WINDOW_CONTEXT, what);
        MIRR_ASSERT_CORE(errorCode != GLFW_INVALID_VALUE, what);
        MIRR_ASSERT_CORE(errorCode != GLFW_INVALID_ENUM, what);

        // GLFW doesn't distinguish between some platform errors that are caused by the programmer. For example, when
        // using multithreading, if we don't detach the context from the old thread, before making it current on the
        // new one, we get a GLFW_PLATFORM_ERROR, which might be not so clear whether it was caused by us or not,
        // because, according to their analysis it can be: a bug or configuration error in GLFW, the underlying 
        // operating system or its drivers, or a lack of required resources.
        switch(errorCode)
        {
            case GLFW_OUT_OF_MEMORY:
                ENGINE_LOG_ERROR("GLFW could not allocate the necessary memory");
                throw std::bad_alloc();

            default:
                throw glfw_error(what);
        }
    }
}
