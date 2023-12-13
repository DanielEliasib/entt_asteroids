#ifndef PHYSICS_PROCESSORS_HPP
#define PHYSICS_PROCESSORS_HPP

#include <raylib.h>
#include <rlgl.h>

#include <components/base.hpp>
#include <entt/entt.hpp>
#include <iostream>
#include <math.hpp>

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
            transform_data.position = transform_data.position + physics_data.velocity * (delta_time / 1000.0f);
            transform_data.rotation = transform_data.rotation + physics_data.angular_velocity * (delta_time / 1000.0f);
        }
    }

   protected:
    entt::registry& registry;
};

#endif // PHYSICS_PROCESSORS_HPP
