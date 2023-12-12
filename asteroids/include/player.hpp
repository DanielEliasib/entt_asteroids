#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <cstdint>
#include <entt/entt.hpp>
#include <render.hpp>

#include "raylib.h"

struct Player
{
    uint8_t id; // INFO: dummy data for now
};

struct transform
{
    Vector2 position;
    Vector2 direction;
    float rotation;
};

struct physics
{
    Vector2 velocity;
    float angular_velocity;
};

entt::entity create_player(entt::registry& registry, uint8_t id)
{
    entt::entity entity = registry.create();
    registry.emplace<Player>(entity, Player{id});
    registry.emplace<transform>(entity, transform{Vector2{400, 300}, Vector2{0, 1}, 0});
    registry.emplace<physics>(entity, physics{Vector2{0, 0}, 0});

    add_render_data(registry, entity, WHITE);

    return entity;
}

#endif // PLAYER_HPP
