#include "Core/OSWindow.h"

#include "Core/Log.h"
#include "Core/Asserts.h"
#include "Core/Utils.h"

#include "Events/WindowEvents.h"
#include "Events/KeyboardEvents.h"
#include "Events/MouseEvents.h"

// So that it doesn't include GL/gl.h (which defines __gl_h_, making Glad complain)
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stb/image.h>

#include <cstdlib>

#if defined(_WIN32) && !defined(DISTRIBUTION_BUILD)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>

    #define WIN32_CONSOLE_WINDOW
#endif

namespace mirras
{
    static void glfwErrorCallback(int32 errorCode, const char* what);

    void OSWindow::init(const WindowSpecs& windowSpecs)
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

        auto title = windowSpecs.title;
        glfwWindowHint(GLFW_VISIBLE, false); // Delay showing the window to avoid seeing a white filled one on Windows

        windowHandle = glfwCreateWindow(width, height, title.empty() ? " " : title.data(), monitor, nullptr);

        // We don't use the size in the specs, because the created window can't always match the requested size
        OSWindow::initialFbSize = getFramebufferSize();

        // GLFW only applies the limits if both minimum/maximum values are different from -1
        glfwSetWindowSizeLimits(windowHandle, windowSpecs.minWidth, windowSpecs.minHeight, -1, -1); // -1 here indicates there's no limits

        if(windowSpecs.keepAspectRatio)
            glfwSetWindowAspectRatio(windowHandle, width, height);

        glfwMakeContextCurrent(windowHandle);

        glfwSwapInterval(windowSpecs.VSync);
        OSWindow::VSyncEnabled = windowSpecs.VSync;

        glfwSetWindowUserPointer(windowHandle, &appCallbacks);

        setGLFWCallbacks();

        if(windowSpecs.iconFilepath.empty())
            return;
        
        if(glfwPlatformSupported(GLFW_PLATFORM_WAYLAND) || glfwPlatformSupported(GLFW_PLATFORM_COCOA))
        {
            ENGINE_LOG_INFO("GLFW doesn't support setting window icons on Wayland or macOS");
            return;
        }

        GLFWimage icon;
        int channels{};

        icon.pixels = stbi_load(windowSpecs.iconFilepath.data(), &icon.width, &icon.height, &channels, 0);

        if(!icon.pixels)
        {
            ENGINE_LOG_ERROR("Unable to load window icon: {}", windowSpecs.iconFilepath);
            return;
        }
        
        glfwSetWindowIcon(windowHandle, 1, &icon);
    }

    void OSWindow::makeContextCurrent(bool makeCurrent) const
    {
        makeCurrent ? glfwMakeContextCurrent(windowHandle) : glfwMakeContextCurrent(nullptr);
    }

    void OSWindow::makeVisible(bool visible) const
    {
        // On some versions of Windows, when the main and console windows are grouped in the taskbar,
        // we make the main window be the first one, so that its icon (if any) is displayed instead
        if(visible)
        {
        #ifdef WIN32_CONSOLE_WINDOW
            auto consoleHandle = GetConsoleWindow();

            char className[19]; // As "ConsoleWindowClass" is the only one we're looking for,
            GetClassNameA(consoleHandle, className, sizeof(className)); // let anything else longer be truncated

            if(std::string{className} == "ConsoleWindowClass")
            {
                ShowWindow(consoleHandle, SW_HIDE);

                glfwShowWindow(windowHandle);

                ShowWindow(consoleHandle, SW_SHOWNOACTIVATE);
            }
        #endif
            // Simplify the code at the cost of calling this function twice (on Windows, non dist builds)
            glfwShowWindow(windowHandle);
        }
        else
            glfwHideWindow(windowHandle);
    }

    bool OSWindow::shouldClose() const
    {
        return glfwWindowShouldClose(windowHandle);
    }

    void OSWindow::swapBuffers() const
    {
        glfwSwapBuffers(windowHandle);
    }

    void OSWindow::waitEvents() const
    {
        glfwWaitEvents();
    }

    void OSWindow::pollEvents() const
    {
        glfwPollEvents();
    }

    void OSWindow::setVSync(bool VSync)
    {
        glfwSwapInterval(VSync);
        OSWindow::VSyncEnabled = VSync;
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
} // namespace mirras
