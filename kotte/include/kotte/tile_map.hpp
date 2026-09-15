#pragma once

#include "kotte/random.hpp"

#include <cstddef>
#include <cstdint>
#include <raylib.h>
#include <vector>

namespace kotte
{
    enum class TileType : std::uint8_t {
        floor,
        wall
    };

    struct Tile final {
        TileType type = TileType::floor;
        std::uint8_t variation = 0;
    };

    // A half-open block of tile indices: [first, last) on both axes.
    // A default-constructed range is empty.
    struct TileRange final {
        int first_x = 0;
        int last_x = 0;
        int first_y = 0;
        int last_y = 0;

        [[nodiscard]] int width() const noexcept;
        [[nodiscard]] int height() const noexcept;
        [[nodiscard]] bool empty() const noexcept;
        [[nodiscard]] std::size_t cell_count() const noexcept;
    };

    class TileMap final {
    public:
        TileMap(int width, int height);

        [[nodiscard]] int width() const noexcept;
        [[nodiscard]] int height() const noexcept;
        [[nodiscard]] std::size_t tile_count() const noexcept;
        [[nodiscard]] bool contains(int x, int y) const noexcept;

        [[nodiscard]] Tile& at(int x, int y);
        [[nodiscard]] const Tile& at(int x, int y) const;

        // Every tile index touched by a world-pixel rectangle, clamped to the
        // map. Pure arithmetic: no tiles are examined.
        [[nodiscard]] TileRange tiles_overlapping(
            Rectangle world_rect, int tile_size) const noexcept;

    private:
        [[nodiscard]] std::size_t index(int x, int y) const;

        int width_;
        int height_;
        std::vector<Tile> tiles_;
    };

    [[nodiscard]] TileMap make_room(int width, int height, Random& random);
}
