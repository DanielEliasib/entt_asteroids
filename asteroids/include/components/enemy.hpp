#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <raylib.h>

#include <entt/entt.hpp>
#include <memory>
#include <utils/state.hpp>

using entt::operator""_hs;

struct enemy_ai
{
    std::shared_ptr<state_machine> ai_machine;
};

static const std::uint32_t enemy_tag = "ENEMY"_hs;

void spawn_random_enemy(entt::registry& registry);
entt::entity spawn_enemy(entt::registry& registry, Vector2 position);

#endif // ENEMY_HPP
