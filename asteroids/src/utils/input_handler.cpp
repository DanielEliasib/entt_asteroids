#include <raylib.h>

#include <math.hpp>
#include <utils/input_handler.hpp>

#include "player.hpp"
#include "raymath.h"

input_handler::input_handler(entt::registry& registry) :
    registry(registry)
{
    acceleration_button_pressed = new acceleration_input_command(registry);
    mouse_moved                 = new mouse_input_command(registry);
}

input_handler::~input_handler()
{
    delete acceleration_button_pressed;
}

void input_handler::handle_input()
{
    if (IsKeyDown(KEY_SPACE) && acceleration_button_pressed != nullptr)
    {
        acceleration_button_pressed->execute(Vector2Zero());
    }

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && acceleration_button_pressed != nullptr)
    {
        auto player_entity    = registry.view<Player, transform>().front();
        auto player_transform = registry.get<transform>(player_entity);
        auto player_physics   = registry.get<physics>(player_entity);

        auto angle           = player_transform.rotation * DEG2RAD;
        auto direction       = Vector2{cos(angle), sin(angle)};
        auto bullet_velocity = player_physics.velocity + direction * 320.0f;
        spawn_bullet(registry, player_transform.position, bullet_velocity);
    }

    if (mouse_moved != nullptr)
    {
        auto mouse_position = GetMousePosition();
        mouse_moved->execute(mouse_position);

		// DrawCircleV(mouse_position, 5, RED);
    }
}
