#pragma once

#include "kotte/camera.hpp"
#include "kotte/entity.hpp"
#include "kotte/entity_store.hpp"
#include "kotte/random.hpp"
#include "kotte/renderer.hpp"
#include "kotte/spatial_grid.hpp"
#include "kotte/tile_map.hpp"
#include "kotte/window.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <raylib.h>
#include <string_view>
#include <vector>

namespace kotte
{
    struct CollisionDiagnostics final {
        std::size_t enemy_updates = 0;
        std::size_t movement_attempts = 0;
        std::size_t spatial_queries = 0;
        std::size_t cells_visited = 0;
        std::size_t candidate_references = 0;
        std::size_t unique_candidates = 0;
        std::size_t exact_tests = 0;
        std::size_t contacts = 0;
        std::size_t boundary_blocks = 0;
        std::size_t blocked_moves = 0;
        std::size_t enemy_turns = 0;
        // Player hurt detection: broad-phase candidates vs confirmed hits.
        std::size_t hurt_candidates = 0;
        std::size_t hurt_exact_tests = 0;
        std::size_t hurt_hits = 0;
    };

    class Application final {
    public:
        Application(
            int window_width,
            int window_height,
            std::string_view title,
            std::uint64_t seed,
            int target_fps = 0);

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        void run();
        void request_exit() noexcept;

    private:
        enum class PlayerState : std::uint8_t {
            alive,
            defeated
        };

        enum class DefeatCause : std::uint8_t {
            blast,
            enemy
        };

        // The round is the authority on whether gameplay may run at all.
        // Won and Lost are terminal: once entered, the round never changes again.
        enum class RoundState : std::uint8_t {
            playing,
            won,
            lost
        };

        struct FrameActions final {
            Vector2 player_movement_direction{};
            bool place_bomb = false;
            bool restart_round = false;
        };

        struct PlaceBombRequest final {
            EntityHandle owner;
            Vector2 world_position{};
            // Captured when the request is made and copied into the bomb, so
            // later pickups never change a bomb that already exists.
            std::uint8_t blast_range = 0;
        };

        struct CollectPickupRequest final {
            EntityHandle pickup;
        };

        struct TileCoordinate final {
            int column = 0;
            int row = 0;
        };

        struct BombDetonated final {
            EntityHandle bomb;
            EntityHandle owner;
            Vector2 world_position{};
            std::uint8_t blast_range = 0;
        };

        struct DestroyEntityRequest final {
            EntityHandle entity;
        };

        // At most one per frame: the first confirmed hit wins and later hits
        // in the same frame collapse into it.
        struct PlayerDefeatRequest final {
            DefeatCause cause = DefeatCause::blast;
        };

        struct BlastEffect final {
            std::vector<Vector2> tile_centres;
            float remaining_seconds = 0.0f;
        };

        void run_frame(float delta_time);
        void begin_frame() noexcept;
        void collect_frame_actions() noexcept;
        void update_gameplay(float delta_time);
        void update_timed_gameplay(float delta_time);
        void resolve_gameplay_facts();
        void apply_structural_mutations();
        void update_presentation(float delta_time);
        void update_player(float delta_time);
        void try_move_player(Vector2 axis_displacement);
        void update_enemies(float delta_time);
        void detect_player_enemy_contact();
        void update_bomb_owner_escapes();
        void detect_player_pickup_contact();
        void apply_pickup_collections();
        void spawn_pickups();
        [[nodiscard]] static bool is_pickup_tile(int column, int row) noexcept;
        void queue_bomb_placement();
        void request_player_defeat(DefeatCause cause) noexcept;
        void apply_player_defeat() noexcept;
        void update_round_state() noexcept;
        void restart_round();
        void apply_entity_destructions();
        void apply_bomb_placements();
        [[nodiscard]] bool resolve_blast_tile(
            BlastEffect& effect,
            int column,
            int row);
        void append_blast_ray(
            BlastEffect& effect,
            int origin_column,
            int origin_row,
            int column_step,
            int row_step,
            std::uint8_t range);
        void update_camera(float delta_time) noexcept;
        void render();
        void populate_round();
        void populate_entities();
        void initialize_enemies();
        [[nodiscard]] Entity& entity(EntityHandle handle) noexcept;
        [[nodiscard]] const Entity& entity(EntityHandle handle) const noexcept;
        [[nodiscard]] Entity& player() noexcept;
        [[nodiscard]] const Entity& player() const noexcept;
        [[nodiscard]] bool player_is_alive() const noexcept;
        [[nodiscard]] bool round_is_playing() const noexcept;
        [[nodiscard]] bool objective_complete() const noexcept;
        [[nodiscard]] static const char* round_state_name(RoundState state) noexcept;
        [[nodiscard]] Rectangle entity_world_bounds(const Entity& entity) const noexcept;
        [[nodiscard]] Rectangle entity_collision_bounds(const Entity& entity) const noexcept;
        [[nodiscard]] Rectangle player_hurt_bounds(const Entity& player_entity) const noexcept;
        [[nodiscard]] bool entity_movement_is_blocked(
            EntityHandle moving_entity_handle,
            Vector2 proposed_position);
        [[nodiscard]] static bool is_solid(EntityKind kind) noexcept;
        [[nodiscard]] bool blocks_mover(
            EntityHandle moving_entity_handle,
            const Entity& candidate) const noexcept;
        [[nodiscard]] bool tile_has_live_bomb(Vector2 world_position) const noexcept;
        [[nodiscard]] static Vector2 cardinal_vector(CardinalDirection direction) noexcept;
        [[nodiscard]] CardinalDirection different_enemy_direction(CardinalDirection current_direction);
        [[nodiscard]] bool owner_has_active_bomb(EntityHandle owner) const noexcept;
        [[nodiscard]] static Vector2 tile_centre(Vector2 world_position) noexcept;
        [[nodiscard]] static Vector2 tile_centre(int column, int row) noexcept;
        [[nodiscard]] static Color entity_color(EntityKind kind) noexcept;
        [[nodiscard]] static float entity_roundness(EntityKind kind) noexcept;
        static constexpr int tile_size_ = 40;
        // Shared by the constructor and restart so both build the same layout.
        static constexpr int map_columns_ = 200;
        static constexpr int map_rows_ = 120;
        static constexpr int spatial_cell_tiles_ = 8;
        static constexpr float spatial_cell_size_ = tile_size_ * spatial_cell_tiles_;
        static constexpr float player_speed_ = 240.0f;
        static constexpr float enemy_speed_ = 80.0f;
        // Solid movers never overlap, so enemy contact is tested against the
        // player's collision footprint grown by this margin.
        static constexpr float player_hurt_margin_ = 4.0f;
        // Objective: defeat this many enemies. Clamped at startup to the number
        // of enemies the seed actually generated, so the round is always winnable.
        static constexpr std::size_t enemies_to_defeat_ = 10;
        static constexpr float bomb_fuse_seconds_ = 2.0f;
        static constexpr std::uint8_t bomb_blast_range_ = 3; // starting range
        // Each pickup adds one tile. Three fixed pickups take 3 to the cap of 6:
        // twice the starting reach, still well short of crossing a screen.
        static constexpr std::uint8_t blast_range_per_pickup_ = 1;
        static constexpr std::uint8_t max_blast_range_ = 6;
        // Fixed pickup tiles near the spawn at (2, 2). The room has no interior
        // walls, so every one is reachable.
        static constexpr TileCoordinate pickup_tiles_[]{
            {6, 2},
            {2, 7},
            {9, 9}
        };
        static constexpr float blast_effect_seconds_ = 0.35f;
        static constexpr Color background_color_{ 0x11, 0x22, 0x33, 0xff };
        static constexpr Color floor_colors_[3]{
            Color{0x38, 0x49, 0x52, 0xff},
            Color{0x3d, 0x4f, 0x59, 0xff},
            Color{0x42, 0x55, 0x60, 0xff}
        };
        static constexpr Color player_color_{ 0xf2, 0xc1, 0x4e, 0xff };
        static constexpr Color defeated_player_color_{ 0x8a, 0x8f, 0x96, 0xff };
        static constexpr Color bomb_color_{ 0x20, 0x24, 0x2a, 0xff };
        static constexpr Color pickup_color_{ 0x5d, 0xd8, 0xe8, 0xff };
        static constexpr Color blast_color_{ 0xf2, 0x8f, 0x3b, 0xe8 };
        static constexpr Color crate_color_{ 0xb0, 0x72, 0x3c, 0xff };
        static constexpr Color enemy_color_{ 0xd9, 0x4f, 0x70, 0xff };
        static constexpr Color wall_color_{ 0x78, 0x5f, 0x47, 0xff };

        Window window_; // Constructed first, destroyed last.
        Camera camera_;
        Random random_;
        TileMap map_;
        EntityStore entities_;
        std::vector<EntityHandle> enemy_handles_;
        std::vector<EntityHandle> active_bomb_handles_;
        SpatialGrid spatial_grid_;
        Renderer renderer_;
        FrameActions frame_actions_;
        std::vector<PlaceBombRequest> place_bomb_requests_;
        std::vector<BombDetonated> bomb_detonated_facts_;
        std::vector<DestroyEntityRequest> destroy_entity_requests_;
        std::vector<CollectPickupRequest> collect_pickup_requests_;
        std::optional<PlayerDefeatRequest> player_defeat_request_;
        std::vector<BlastEffect> blast_effects_;
        CollisionDiagnostics collision_diagnostics_;
        std::uint64_t seed_;
        EntityHandle player_handle_;
        PlayerState player_state_ = PlayerState::alive;
        DefeatCause defeat_cause_ = DefeatCause::blast;
        std::size_t player_defeats_ = 0;
        RoundState round_state_ = RoundState::playing;
        std::size_t enemy_target_ = 0;
        std::size_t enemies_defeated_ = 0;
        std::size_t round_number_ = 1;
        std::uint8_t player_blast_range_ = bomb_blast_range_;
        bool exit_requested_ = false;
    };
}
