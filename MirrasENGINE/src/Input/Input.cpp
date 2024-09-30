#include "Input/Input.h"

#include "Core/Application.h"

namespace mirras
{
    bool Input::isKeyDown(Key key)
    {
        int32 state = glfwGetKey(App::getOSWindow(), static_cast<int32>(key));
        return KeyState{state} == KeyState::Pressed;
    }

    bool Input::isMouseButtonDown(Mouse button)
    {
        int32 state = glfwGetMouseButton(App::getOSWindow(), static_cast<int32>(button));
        return MouseButtonState{state} == MouseButtonState::Pressed;
    }
} // namespace mirras
