#ifndef SCENE_MANAGEMENT
#define SCENE_MANAGEMENT

#include <entt/entt.hpp>
#include <memory>

#include "components/asteroid.hpp"
#include "processors/base_processors.hpp"
#include "processors/enemy_processors.hpp"
#include "processors/physics_processors.hpp"
#include "processors/render_processors.hpp"
#include "raylib.h"
#include "utils/input_handler.hpp"
#include "utils/state.hpp"

static const Color background_color = {15, 15, 15, 255};
static const Color text_color       = {204, 191, 147, 255};

static const state create_title_scene()
{
    // state title_scene;
    // title_scene.title       = "Title Scene";
    // title_scene.description = "This is the title scene";
    // title_scene.options     = {
    //     {"Start Game", "start_game"},
    //     {"Exit", "exit"}};
    return state(nullptr, nullptr, nullptr);
}

// struct game_scene_state : state
// {
//     game_scene_state() :
//         state(nullptr, nullptr, nullptr),
//         registry(), input(registry), general_scheduler(), render_scheduler(), cleanup_scheduler()
//     {
//     }
//
//    private:
//     void _on_enter_func()
//     {
//         general_scheduler.attach<lifetime_process>(registry);
//         general_scheduler.attach<enemy_ai_process>(registry);
//         general_scheduler.attach<physics_process>(registry);
//         general_scheduler.attach<collision_process>(registry);
//         general_scheduler.attach<boundary_process>(registry);
//
//         render_scheduler.attach<ui_process>(registry);
//         render_scheduler.attach<sprite_render_process>(registry);
//         render_scheduler.attach<sprite_sequence_process>(registry);
//         render_scheduler.attach<shape_render_process>(registry);
//
//         cleanup_scheduler.attach<cleanup_process>(registry);
//
//         // INFO: Load textures
//         LoadTextureToEntity<GAME_TEXTURES::MAINTEXTURE>("asteroids/resources/simpleSpace_tilesheet.png", registry);
//         LoadTextureToEntity<GAME_TEXTURES::PLANETEXTURE>("asteroids/resources/simplePlanes_tilesheet.png", registry);
//
//         // INFO: Create player
//         spawn_main_camera(registry);
//         create_player(registry, 0);
//
//         // INFO: Create enemies
//         spawn_random_asteroid_distribution(registry, 4);
//         spawn_random_enemy(registry);
//     }
//
//     void _on_exit_func()
//     {
//         auto texture_view = registry.view<Texture2D>();
//
//         for (auto [entity, texture] : texture_view.each())
//         {
//             UnloadTexture(texture);
//         }
//
//         registry.clear();
//     }
//
//     void _on_update_func(float delta_time)
//     {
//         const uint32_t delta_time_ms = delta_time * 1000;
//
//         input.handle_input();
//
//         general_scheduler.update(delta_time);
//
//         BeginDrawing();
//         ClearBackground(background_color);
//
//         Camera2D camera = registry.get<Camera2D>(registry.view<Camera2D>().front());
//         BeginMode2D(camera);
//
//         render_scheduler.update(delta_time);
//
//         EndMode2D();
//         EndDrawing();
//
//         cleanup_scheduler.update(delta_time);
//     }
//
//    protected:
//     entt::registry registry;
//     input_handler input;
//     entt::scheduler general_scheduler;
//     entt::scheduler render_scheduler;
//     entt::scheduler cleanup_scheduler;
// };

inline const void create_game_scene(std::shared_ptr<state>& scene_state)
{
    std::shared_ptr<entt::registry> registry = std::make_shared<entt::registry>();

    std::shared_ptr<input_handler> input = std::make_shared<input_handler>(*registry);

    std::shared_ptr<entt::scheduler> general_scheduler = std::make_shared<entt::scheduler>();
    std::shared_ptr<entt::scheduler> render_scheduler  = std::make_shared<entt::scheduler>();
    std::shared_ptr<entt::scheduler> cleanup_scheduler = std::make_shared<entt::scheduler>();

    auto on_enter = [registry, input, general_scheduler, render_scheduler, cleanup_scheduler]() {
        general_scheduler->attach<lifetime_process>(*registry);
        general_scheduler->attach<enemy_ai_process>(*registry);
        general_scheduler->attach<physics_process>(*registry);
        general_scheduler->attach<collision_process>(*registry);
        general_scheduler->attach<boundary_process>(*registry);

        render_scheduler->attach<ui_process>(*registry);
        render_scheduler->attach<sprite_render_process>(*registry);
        render_scheduler->attach<sprite_sequence_process>(*registry);
        render_scheduler->attach<shape_render_process>(*registry);

        cleanup_scheduler->attach<cleanup_process>(*registry);

        // INFO: Load textures
        LoadTextureToEntity<GAME_TEXTURES::MAINTEXTURE>("asteroids/resources/simpleSpace_tilesheet.png", *registry);
        LoadTextureToEntity<GAME_TEXTURES::PLANETEXTURE>("asteroids/resources/simplePlanes_tilesheet.png", *registry);

        // INFO: Create player
        spawn_main_camera(*registry);
        create_player(*registry, 0);

        // INFO: Create enemies
        spawn_random_asteroid_distribution(*registry, 4);
        spawn_random_enemy(*registry);
    };

    auto on_exit = [registry]() {
        auto texture_view = registry->view<Texture2D>();

        for (auto [entity, texture] : texture_view.each())
        {
            UnloadTexture(texture);
        }

        registry->clear();
    };

    auto on_update = [registry, input, general_scheduler, render_scheduler, cleanup_scheduler](float delta_time) {
        const uint32_t delta_time_ms = delta_time * 1000;

        input->handle_input();

        general_scheduler->update(delta_time_ms);

        BeginDrawing();
        ClearBackground(background_color);

        Camera2D camera = registry->get<Camera2D>(registry->view<Camera2D>().front());
        BeginMode2D(camera);

        render_scheduler->update(delta_time_ms);

        EndMode2D();
        EndDrawing();

        cleanup_scheduler->update(delta_time_ms);
    };

    scene_state = std::make_shared<state>(on_enter, on_exit, on_update);
}

static const state_machine create_game_state_machine()
{
    // std::shared_ptr<state> title_scene = std::make_shared<state>(create_title_scene());
    std::shared_ptr<state> game_scene;
    create_game_scene(game_scene);

    state_machine game_state_machine(game_scene);

    // game_state_machine.add_transition(title_scene, game_scene, "start_game", []() { return true; });
    // game_state_machine.add_transition(game_scene, title_scene, "exit", []() { return true; });

    return game_state_machine;
}

#endif // SCENE_MANAGEMENT
