#include <raylib.h>

#include <math.hpp>
#include <utils/input_handler.hpp>

#include "components/player.hpp"
#include "components/render.hpp"
#include "raymath.h"

input_handler::input_handler(entt::registry& registry) :
    registry(registry)
{
    acceleration_button_pressed = new acceleration_input_command(registry);
    mouse_moved                 = new mouse_input_command(registry);
    shoot_button_pressed        = new shoot_input_command(registry);
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
        shoot_button_pressed->execute(Vector2Zero());
    }

    if (mouse_moved != nullptr)
    {
        auto mouse_position = GetMousePosition();
        mouse_moved->execute(mouse_position);

        // DrawCircleV(mouse_position, 5, RED);
    }
}
