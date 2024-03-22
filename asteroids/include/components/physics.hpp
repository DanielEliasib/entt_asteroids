#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include <entt/entt.hpp>

struct circle_collider
{
    float radius;

    entt::delegate<void(entt::registry&, entt::entity, entt::entity)> on_collision;
};

struct bullet_collision_response
{
    entt::delegate<void(entt::registry&, entt::entity, entt::entity)> on_collision;
};

struct asteroid_collision_response
{
    entt::delegate<void(entt::registry&, entt::entity, entt::entity)> on_collision;
};

#endif // PHYSICS_HPP
