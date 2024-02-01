// #include <components/base.hpp>
#include <iostream>
#include <math.hpp>
#include <player.hpp>
#include <utils/command.hpp>

#include "raymath.h"

void acceleration_input_command::execute(Vector2 input)
{
    std::cout << "Acceleration button pressed" << std::endl;

    auto physics_view = registry.view<Player, physics, transform>();
    for (auto [entity, player_data, physics_data, transform_data] : physics_view.each())
    {
        // TODO: Change this so it is generated automatically every frame
        Vector2 direction             = Vector2Transform(Vector2{1, 0}, MatrixRotateZ(transform_data.rotation * DEG2RAD));
        physics_data.external_impulse = physics_data.external_impulse + direction * 120.0f;
    }
}

void mouse_input_command::execute(Vector2 input)
{
	// PERF: Maybe some of these entities should be cached
    auto physics_view = registry.view<Player, physics, transform>();
    auto camera_view  = registry.view<Camera2D>();
    auto player_view  = registry.view<Player, transform>();

    auto player_entity = player_view.front();
    auto camera_entity = camera_view.front();

    auto player_transform = player_view.get<transform>(player_entity);
    auto camera_data      = camera_view.get<Camera2D>(camera_entity);

    auto player_position_transformed = GetWorldToScreen2D(player_transform.position, camera_data);

    for (auto [entity, player_data, physics_data, transform_data] : physics_view.each())
    {
        Vector2 mouse_position  = Vector2{input.x, input.y};
        Vector2 player_position = player_position_transformed;
        Vector2 direction       = Vector2Normalize(Vector2Subtract(mouse_position, player_position));
        float angle             = atan2(direction.y, direction.x) * RAD2DEG;
        transform_data.rotation = angle;
    }
}
