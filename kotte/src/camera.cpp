#include "camera.hpp"

namespace kotte
{
    namespace
    {
        [[nodiscard]] Vector2 half(Vector2 v) noexcept
        {
            return { v.x * 0.5f, v.y * 0.5f };
        }
    }

    Camera::Camera(Vector2 viewport_size) noexcept
        : viewport_size_{ viewport_size }
    {
    }

    void Camera::set_centre(Vector2 world_position) noexcept
    {
        centre_ = world_position;
    }

    void Camera::set_viewport_size(Vector2 viewport_size) noexcept
    {
        viewport_size_ = viewport_size;
    }

    Vector2 Camera::world_to_screen(Vector2 world_position) const noexcept
    {
        const Vector2 h = half(viewport_size_);
        return {
            world_position.x - centre_.x + h.x,
            world_position.y - centre_.y + h.y
        };
    }

    Rectangle Camera::world_view() const noexcept
    {
        const Vector2 h = half(viewport_size_);
        return {
            centre_.x - h.x,
            centre_.y - h.y,
            viewport_size_.x,
            viewport_size_.y
        };
    }
}
