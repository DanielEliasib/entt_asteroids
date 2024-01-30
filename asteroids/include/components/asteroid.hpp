#ifndef ASTEROID_HPP
#define ASTEROID_HPP

#include <stdint.h>

#include <entt/entt.hpp>

#include "raymath.h"


struct asteroid
{
    // INFO: 0 = small, 1 = medium, 2 = large
    int8_t level;

    entt::delegate<void(entt::registry&, entt::entity, Vector2, int)> on_asteroid_death;
};

#endif // ASTEROID_HPP
