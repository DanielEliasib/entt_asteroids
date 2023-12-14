// #include <components/base.hpp>
#include <iostream>
#include <math.hpp>
#include <player.hpp>
#include <utils/events.hpp>

void acceleration_button_pressed_event::execute()
{
    std::cout << "Acceleration button pressed" << std::endl;

    auto physics_view = registry.view<Player, physics>();
    for (auto [entity, player_data, physics_data] : physics_view.each())
    {
        physics_data.external_impulse = physics_data.external_impulse + Vector2{0, -100};
    }
}
