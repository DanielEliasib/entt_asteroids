#ifndef ASTEROID_HPP
#define ASTEROID_HPP

#include <raylib.h>
#include <stdint.h>

#include <entt/entt.hpp>

struct asteroid
{
    // INFO: 0 = small, 1 = medium, 2 = large
    int8_t level;

    entt::delegate<void(entt::registry&, entt::entity, Vector2, int)> on_asteroid_death;
};

entt::entity spawn_star(entt::registry& registry, Vector2 position, float angle);
entt::entity spawn_asteroid(entt::registry& registry, Vector2 position, Vector2 velocity, int8_t level);

entt::entity spawn_smoke_explosion(entt::registry& registry, Vector2 position, int id, float radius, float duration);
void spawn_random_asteroid_distribution(entt::registry& registry, int count);
void spawn_random_start_distribution(entt::registry& registry, int count);
#endif // ASTEROID_HPP
