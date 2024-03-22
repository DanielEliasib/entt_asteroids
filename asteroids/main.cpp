#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <array>
#include <components/player.hpp>
#include <entt/entt.hpp>
#include <math.hpp>
#include <memory>
#include <processors/physics_processors.hpp>
#include <processors/render_processors.hpp>

#include "components/asteroid.hpp"
#include "components/base.hpp"
#include "components/enemy.hpp"
#include "components/render.hpp"
#include "processors/base_processors.hpp"
#include "processors/enemy_processors.hpp"
#include "utils/input_handler.hpp"
#include "utils/state.hpp"

using entt::operator""_hs;

void state_machine_test()
{
    int print_count = 0;

    auto print_enter = [&print_count]() {
        std::cout << "Enter printing state" << std::endl;
        print_count = 0;
    };
    auto print_update = [&print_count](float delta_time) {
        std::cout << "Update printing state" << std::endl;
        print_count++;
    };
    auto print_exit = []() { std::cout << "Exit printing state" << std::endl; };

    auto print_condition = [&print_count]() { return print_count > 3; };

    auto warning_enter = [&print_count]() {
        std::cout << "Enter WARNING state" << std::endl;
        print_count = 0;
    };
    auto warning_update = [&print_count](float delta_time) {
        std::cout << "Update WARNING state" << std::endl;
        print_count++;
    };
    auto warning_exit = []() { std::cout << "Exit WARNING state" << std::endl; };

    auto warning_condition = [&print_count]() { return print_count > 6; };

    std::shared_ptr<state> printing_state = std::make_shared<state>(print_enter, print_exit, print_update);
    std::shared_ptr<state> warning_state  = std::make_shared<state>(warning_enter, warning_exit, warning_update);

    printing_state->add_transition(print_condition, warning_state);
    warning_state->add_transition(warning_condition, printing_state);

    auto machine = state_machine(printing_state);

    for (int i = 0; i < 13; i++)
    {
        machine.update(1);
    }

    std::cout << "End of state machine test" << std::endl;
}

int main()
{
    const char* TITLE = "ASTEROIDS";
    InitWindow(800, 800, TITLE);
    SetTargetFPS(60);

    Color background_color = {15, 15, 15, 255};
    Color text_color       = {204, 191, 147, 255};

    entt::registry registry;

    entt::scheduler general_scheduler;
    general_scheduler.attach<lifetime_process>(registry);
    general_scheduler.attach<enemy_ai_process>(registry);
    general_scheduler.attach<physics_process>(registry);
    general_scheduler.attach<collision_process>(registry);
    general_scheduler.attach<boundary_process>(registry);

    entt::scheduler render_scheduler;
    render_scheduler.attach<sprite_sequence_process>(registry);
    render_scheduler.attach<sprite_render_process>(registry);
    render_scheduler.attach<shape_render_process>(registry);
    render_scheduler.attach<ui_process>(registry);
    // render_scheduler.attach<camera_process>(registry);
    //

    entt::scheduler cleanup_scheduler;
    cleanup_scheduler.attach<cleanup_process>(registry);

    LoadTextureToEntity<GAME_TEXTURES::MAINTEXTURE>("asteroids/resources/simpleSpace_tilesheet.png", registry);
    LoadTextureToEntity<GAME_TEXTURES::PLANETEXTURE>("asteroids/resources/simplePlanes_tilesheet.png", registry);

    create_player(registry, 0);

    input_handler input_handler(registry);

    spawn_main_camera(registry);

    spawn_random_asteroid_distribution(registry, 4);

    spawn_random_enemy(registry);

    while (!WindowShouldClose())
    {
        const uint32_t delta_time = GetFrameTime() * 1000;

        input_handler.handle_input();

        general_scheduler.update(delta_time);

        BeginDrawing();
        ClearBackground(background_color);

        Camera2D camera = registry.get<Camera2D>(registry.view<Camera2D>().front());
        BeginMode2D(camera);

        render_scheduler.update(delta_time);

        EndMode2D();
        EndDrawing();

        cleanup_scheduler.update(delta_time);
    }

    return 0;
}
