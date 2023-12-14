#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <iostream>
#include <math.hpp>
#include <player.hpp>
#include <processors/physics_processors.hpp>
#include <processors/render_processors.hpp>

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
    general_scheduler.attach<physics_process>(registry);

    entt::scheduler render_scheduler;
    render_scheduler.attach<render_process>(registry);

    create_player(registry, 0);

    input_handler input_handler(registry);

    while (!WindowShouldClose())
    {
        const uint32_t delta_time = GetFrameTime() * 1000;

        input_handler.handle_input();

        general_scheduler.update(delta_time);

        BeginDrawing();
        ClearBackground(background_color);
        DrawText(TITLE, 10, 10, 20, text_color);
        DrawFPS(10, 30);

        render_scheduler.update(delta_time);

        EndDrawing();
    }

    return 0;
}
