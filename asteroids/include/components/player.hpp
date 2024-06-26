#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <raylib.h>
#include <raymath.h>

#include <cstdint>
#include <entt/entt.hpp>

#include "components/base.hpp"

using entt::operator""_hs;

struct Player
{
    uint8_t id; // INFO: dummy data for now
    uint32_t score;
    uint8_t lives;

    bool game_over = false;
};

static const std::uint32_t player_tag       = "PLAYER"_hs;
static const std::uint32_t player_trail_tag = "PLAYER_TRAIL"_hs;

entt::entity create_player(entt::registry& registry, uint8_t id);

entt::entity spawn_bullet(entt::registry& registry, Vector2 position, Vector2 velocity, const team& bullet_team);

entt::entity spawn_explosion(entt::registry& registry, Vector2 position, float scale);

void spawn_game_ui(entt::registry& registry);

void spawn_game_over(entt::registry& registry);
#endif // PLAYER_HPP
