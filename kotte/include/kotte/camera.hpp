#pragma once

#include <raylib.h>

namespace kotte
{
    // A translation-only view: world and screen pixels have the same scale.
    // Application decides what the camera follows.
    class Camera final {
    public:
        // Viewport width and height, in screen pixels.
        explicit Camera(Vector2 viewport_size) noexcept;

        // The world-space point at the centre of the view.
        void set_centre(Vector2 world_position) noexcept;

        void set_viewport_size(Vector2 viewport_size) noexcept;

        // Convert a world-pixel position to a screen-pixel position.
        [[nodiscard]] Vector2 world_to_screen(
            Vector2 world_position) const noexcept;

        // Describe the visible rectangle in world pixels.
        [[nodiscard]] Rectangle world_view() const noexcept;

    private:
        Vector2 centre_{};
        Vector2 viewport_size_{};
    };
}
