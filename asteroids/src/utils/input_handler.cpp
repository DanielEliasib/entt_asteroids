#include <raylib.h>

#include <utils/input_handler.hpp>

input_handler::input_handler(entt::registry& registry) :
    registry(registry)
{
    acceleration_button_pressed = new acceleration_button_pressed_event(registry);
}

input_handler::~input_handler()
{
    delete acceleration_button_pressed;
}

void input_handler::handle_input()
{
    if (IsKeyDown(KEY_SPACE) && acceleration_button_pressed != nullptr)
    {
        acceleration_button_pressed->execute();
    }
}
