#include "kotte/application.hpp"

#include <cassert>
#include <cstddef>

namespace kotte
{
    namespace
    {
        constexpr int tile_size = 40;
    }

    Application::Application
    (
        int width,
        int height,
        std::string_view title,
        std::uint64_t seed,
        int target_fps
    )
        : window_{ width, height, title, target_fps }
        , random_{ seed }
        , map_{ make_room(200, 120, random_) }
        , camera_{ window_.size() }
        , seed_{ seed } {
    }

    void Application::run() // Function controls the entire game loop
    {
        while (!window_.should_close() && !exit_requested_) {
            update(GetFrameTime());
            render();
        }
    }

    void Application::update(float dt)
    {
        if (IsKeyDown(KEY_ESCAPE) || IsKeyDown(KEY_Q)) {
            request_exit();
        }

        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
            try_move_player(-1, 0);
        }
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
            try_move_player(1, 0);
        }
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
            try_move_player(0, -1);
        }
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
            try_move_player(0, 1);
        }

        camera_.set_viewport_size(window_.size());
        camera_.set_centre(player_world_centre());

        (void)dt; //Q&D: avoid warning about unused argument.
    }


    // Update and Run are the 2 phasess that run each frame


    void Application::render() const
    {
        Frame frame{ background_color_ };

        constexpr Color floor_colors[]{
            Color{0x38, 0x49, 0x52, 0xff},
            Color{0x3d, 0x4f, 0x59, 0xff},
            Color{0x42, 0x55, 0x60, 0xff}
        };

        const TileRange range = map_.tiles_overlapping(camera_.world_view(), tile_size);

        std::size_t drawn = 0;

        for (int y = range.first_y; y < range.last_y; ++y) {
            for (int x = range.first_x; x < range.last_x; ++x) {
                const Tile& tile = map_.at(x, y);
                const Color color = tile.type == TileType::wall
                    ? Color{ 0x78, 0x5f, 0x47, 0xff }
                : floor_colors[tile.variation % 3];

                const Vector2 screen = camera_.world_to_screen({
                    static_cast<float>(x * tile_size),
                    static_cast<float>(y * tile_size)
                    });

                DrawRectangle(
                    static_cast<int>(screen.x),
                    static_cast<int>(screen.y),
                    tile_size - 1,
                    tile_size - 1,
                    color);

                ++drawn;
            }
        }

        assert(drawn == range.cell_count());

        const Vector2 player_screen = camera_.world_to_screen(player_world_centre());

        DrawCircle(
            static_cast<int>(player_screen.x),
            static_cast<int>(player_screen.y),
            tile_size * 0.3f,
            Color{ 0xf2, 0xc1, 0x4e, 0xff });

        DrawFPS(GetScreenWidth() - 100, 2);
        DrawText(TextFormat("seed: %llu", static_cast<unsigned long long>(seed_)), 10, 10, 20, RAYWHITE);
        DrawText(TextFormat("tiles: %zu / %zu drawn", drawn, map_.tile_count()), 10, 34, 20, RAYWHITE);
        DrawText("move: WASD/arrows | quit: Q/Escape", 10, window_.height() - 30, 20, LIGHTGRAY);
    }

    void Application::try_move_player(int delta_x, int delta_y) {
        const int destination_x = player_x_ + delta_x;
        const int destination_y = player_y_ + delta_y;

        if (map_.contains(destination_x, destination_y)
            && map_.at(destination_x, destination_y).type != TileType::wall) {
            player_x_ = destination_x;
            player_y_ = destination_y;
        }
    }

    Vector2 Application::player_world_centre() const noexcept
    {
        return {
            static_cast<float>(player_x_ * tile_size + tile_size / 2),
            static_cast<float>(player_y_ * tile_size + tile_size / 2)
        };
    }

    void Application::request_exit() noexcept {
        exit_requested_ = true;
    }

} //kotte
