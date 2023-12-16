#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <components/base.hpp>
#include <components/render.hpp>
#include <cstdint>
#include <entt/entt.hpp>

#include "raylib.h"

struct Player
{
    uint8_t id; // INFO: dummy data for now
};

entt::entity create_player(entt::registry& registry, uint8_t id);
entt::entity spawn_bullet(entt::registry& registry, Vector2 position, Vector2 velocity);

#endif // PLAYER_HPP
