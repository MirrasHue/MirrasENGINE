#include "Core/Renderer/Camera2D.h"

#include "Core/Application.h"

#include "Events/WindowEvents.h"

namespace mirras
{
    Camera2D::Camera2D(vec2f _position, vec2f _offset, float _rotation, float _zoom)
        : position{_position}, rotation{_rotation}, zoom{_zoom}
    {
        setOffset(_offset.x, _offset.y);
    }

    void Camera2D::setOffset(float xOffset, float yOffset)
    {
        auto [currentWidth, currentHeight] = App::getOSWindow().getFramebufferSize();
        static auto [initialWidth, initialHeight] = App::getOSWindow().getInitialFbSize();

        offsetRatio.x = xOffset / initialWidth;
        offsetRatio.y = yOffset / initialHeight;

        offset.x = offsetRatio.x * currentWidth;
        offset.y = offsetRatio.y * currentHeight;
    }

    void Camera2D::setOffsetX(float xOffset)
    {
        setOffset(xOffset, offset.y);
    }

    void Camera2D::setOffsetY(float yOffset)
    {
        setOffset(offset.x, yOffset);
    }

    void Camera2D::onWindowResize(Event& event)
    {
        if(!Event::is_a<WindowResized>(event))
            return;

        auto[currentWidth, currentHeight] = static_cast<WindowResized&>(event).windowSize;

        static int32 initialFbHeight = App::getOSWindow().getInitialFbSize().y;

        zoom = (float)currentHeight / initialFbHeight;

        offset.x = offsetRatio.x * currentWidth;
        offset.y = offsetRatio.y * currentHeight;
    }
} // namespace mirras
