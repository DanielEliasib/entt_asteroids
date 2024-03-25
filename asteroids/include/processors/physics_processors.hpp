#ifndef PHYSICS_PROCESSORS_HPP
#define PHYSICS_PROCESSORS_HPP

#include <raylib.h>
#include <rlgl.h>

#include <components/base.hpp>
#include <entt/entt.hpp>
#include <iostream>
#include <math.hpp>

#include "components/physics.hpp"
#include "raymath.h"

struct physics_process : entt::process<physics_process, uint32_t>
{
    using delta_type = std::uint32_t;

    physics_process(entt::registry& registry) :
        registry(registry) {}

    void update(delta_type delta_time, void*)
    {
        auto physics_view = registry.view<transform, physics>();
        for (auto [entity, transform_data, physics_data] : physics_view.each())
        {
            physics_data.velocity = physics_data.velocity + physics_data.external_impulse * (delta_time / 1000.0f);
            if (physics_data.drag > 0.0f)
            {
                physics_data.velocity = physics_data.velocity * (1.0f - physics_data.drag);
            }

            transform_data.position = transform_data.position + physics_data.velocity * (delta_time / 1000.0f);
            transform_data.rotation = transform_data.rotation + physics_data.angular_velocity * (delta_time / 1000.0f);

            physics_data.external_impulse = Vector2{0, 0};
        }
    }

   protected:
    entt::registry& registry;
};

struct collision_process : entt::process<collision_process, uint32_t>
{
    using delta_type = std::uint32_t;

    collision_process(entt::registry& registry) :
        registry(registry) {}

    void update(delta_type delta_time, void*)
    {
        auto collider_view = registry.view<transform, circle_collider>();

        for (auto [entity, transform_data, collision_data] : collider_view.each())
        {
            for (auto [other_entity, other_transform_data, other_collision_data] : collider_view.each())
            {
                if (entity == other_entity)
                    continue;

                auto distance = Vector2DistanceSqr(transform_data.position, other_transform_data.position);
                if (distance > collision_data.radius * collision_data.radius + other_collision_data.radius * other_collision_data.radius)
                    continue;

                if (!collision_data.on_collision)
                    continue;

                if (!registry.valid(entity) || !registry.valid(other_entity))
                    continue;

                collision_data.on_collision(registry, entity, other_entity);
            }
        }
    }

   protected:
    entt::registry& registry;
};

#endif // PHYSICS_PROCESSORS_HPP
