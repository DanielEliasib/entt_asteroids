#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <components/player.hpp>
#include <entt/entt.hpp>
#include <math.hpp>
#include <processors/physics_processors.hpp>
#include <processors/render_processors.hpp>

#include "scenes/scene_management.hpp"
#include "utils/state.hpp"


int main()
{
    const char* TITLE = "ASTEROIDS";
    InitWindow(800, 800, TITLE);
    SetTargetFPS(60);

    auto game_machine = create_game_state_machine();
    game_machine.start();

    while (!WindowShouldClose())
    {
        const float delta_time = GetFrameTime();

        game_machine.update(delta_time);
    }

    game_machine.stop();

    CloseWindow();

    return 0;
}
