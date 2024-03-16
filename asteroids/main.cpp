#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <components/player.hpp>
#include <math.hpp>
#include <processors/physics_processors.hpp>
#include <processors/render_processors.hpp>

#include "components/asteroid.hpp"
#include "components/render.hpp"
#include "processors/base_processors.hpp"
#include "utils/input_handler.hpp"

int main()
{
    const char* TITLE = "ASTEROIDS";
    InitWindow(800, 600, TITLE);
    SetTargetFPS(60);

    Color background_color = {15, 15, 15, 255};
    Color text_color       = {204, 191, 147, 255};

    entt::registry registry;

    entt::scheduler general_scheduler;
    general_scheduler.attach<lifetime_process>(registry);
    general_scheduler.attach<physics_process>(registry);
    general_scheduler.attach<collision_process>(registry);
    general_scheduler.attach<boundary_process>(registry);

    entt::scheduler render_scheduler;
    render_scheduler.attach<sprite_render_process>(registry);
    render_scheduler.attach<shape_render_process>(registry);
    render_scheduler.attach<ui_process>(registry);
    // render_scheduler.attach<camera_process>(registry);

    LoadTextureToEntity("asteroids/resources/simpleSpace_tilesheet.png", registry);
    create_player(registry, 0);

    input_handler input_handler(registry);

    // Add a raylib camera

    spawn_main_camera(registry);

    spawn_random_asteroid_distribution(registry, 4);

    while (!WindowShouldClose())
    {
        const uint32_t delta_time = GetFrameTime() * 1000;

        input_handler.handle_input();

        general_scheduler.update(delta_time);

        BeginDrawing();
        ClearBackground(background_color);

        Camera2D camera = registry.get<Camera2D>(registry.view<Camera2D>().front());
        BeginMode2D(camera);

        DrawText(TITLE, 10, 10, 20, text_color);
        DrawText("Press [ESC] to exit", 10, 50, 10, text_color);
        DrawFPS(10, 30);

        render_scheduler.update(delta_time);

        EndMode2D();
        EndDrawing();
    }

    return 0;
}
