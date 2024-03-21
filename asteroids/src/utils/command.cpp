// #include <components/base.hpp>
#include <components/player.hpp>
#include <iostream>
#include <teams.hpp>
#include <utils/command.hpp>

#include "math.hpp"
#include "raymath.h"

void acceleration_input_command::execute(Vector2 input)
{
    std::cout << "Acceleration button pressed" << std::endl;

    auto physics_view = registry.view<entt::tag<player_tag>, physics, transform>();
    for (auto [entity, physics_data, transform_data] : physics_view.each())
    {
        // TODO: Change this so it is generated automatically every frame
        Vector2 direction             = Vector2Transform(Vector2{1, 0}, MatrixRotateZ(transform_data.rotation * DEG2RAD));
        physics_data.external_impulse = physics_data.external_impulse + direction * 120.0f;
    }
}

void mouse_input_command::execute(Vector2 input)
{
    // PERF: Maybe some of these entities should be cached
    auto physics_view = registry.view<entt::tag<player_tag>, physics, transform>();
    auto camera_view  = registry.view<Camera2D>();

    auto player_entity = physics_view.front();
    auto camera_entity = camera_view.front();

    if (!registry.valid(player_entity) || !registry.valid(camera_entity))
        return;

    auto player_transform = physics_view.get<transform>(player_entity);
    auto camera_data      = camera_view.get<Camera2D>(camera_entity);

    auto player_position_transformed = GetWorldToScreen2D(player_transform.position, camera_data);

    for (auto [entity, physics_data, transform_data] : physics_view.each())
    {
        Vector2 mouse_position  = Vector2{input.x, input.y};
        Vector2 player_position = player_position_transformed;
        Vector2 direction       = Vector2Normalize(Vector2Subtract(mouse_position, player_position));
        float angle             = atan2(direction.y, direction.x) * RAD2DEG;
        transform_data.rotation = angle;
    }
}

void shoot_input_command::execute(Vector2 input)
{
    auto player_view   = registry.view<entt::tag<player_tag>, transform, physics>();
    auto player_entity = player_view.front();

    if (!registry.valid(player_entity))
        return;

    auto now = std::chrono::high_resolution_clock::now();

    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - _last_shot_time).count();

    if (duration_ms < _cooldown)
    {
        return;
    }

    if (duration_ms > _long_cooldown)
    {
        _shots_fired = 0;
    }

    if (_cooldown == _long_cooldown)
    {
        _cooldown = _short_cooldown;
    }

    auto player_transform = registry.get<transform>(player_entity);
    auto player_physics   = registry.get<physics>(player_entity);

    auto angle           = player_transform.rotation * DEG2RAD;
    auto direction       = Vector2{cos(angle), sin(angle)};
    auto bullet_velocity = player_physics.velocity + direction * 320.0f;
    spawn_bullet<team_player_tag>(registry, player_transform.position, bullet_velocity);

    _shots_fired++;

    if (_shots_fired >= _max_shots)
    {
        _shots_fired = 0;

        _cooldown = _long_cooldown;
    }

    _last_shot_time = now;
}
