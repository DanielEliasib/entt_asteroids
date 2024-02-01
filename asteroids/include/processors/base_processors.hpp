#ifndef BASE_PROCESSORS_HPP
#define BASE_PROCESSORS_HPP

#include <entt/entt.hpp>
#include <iostream>

#include "components/base.hpp"
#include "player.hpp"
#include "raylib.h"
#include "raymath.h"

struct lifetime_process : entt::process<lifetime_process, std::uint32_t>
{
    using delta_type = std::uint32_t;

    lifetime_process(entt::registry& registry) :
        registry(registry) {}

    void update(delta_type delta_time, void*)
    {
        auto lifetime_view = registry.view<lifetime>();

        for (auto [entity, lifetime_data] : lifetime_view.each())
        {
            if (lifetime_data.elapsed >= lifetime_data.lifetime)
            {
                registry.destroy(entity);
                continue;
            }

            lifetime_data.elapsed += delta_time / 1000.0f;
        }
    }

   protected:
    entt::registry& registry;
};

struct boundary_process : entt::process<boundary_process, std::uint32_t>
{
    using delta_type = std::uint32_t;

    boundary_process(entt::registry& registry) :
        registry(registry) {}

    void update(delta_type delta_time, void*)
    {
        auto boundable_view = registry.view<transform>();
        auto camera_view    = registry.view<Camera2D>();

        auto camera_entity = camera_view.front();
        auto camera_data   = camera_view.get<Camera2D>(camera_entity);

        for (auto [entity, transform_data] : boundable_view.each())
        {
            auto screen_position = GetWorldToScreen2D(transform_data.position, camera_data);
            bool has_changed     = false;

            if (screen_position.x <= 0)
            {
                screen_position.x = GetScreenWidth();
                has_changed       = true;
            } else if (screen_position.x >= GetScreenWidth())
            {
                screen_position.x = 0;
                has_changed       = true;
            }

            if (screen_position.y <= 0)
            {
                screen_position.y = GetScreenHeight();
                has_changed       = true;
            } else if (screen_position.y >= GetScreenHeight())
            {
                screen_position.y = 0;
                has_changed       = true;
            }

            if (has_changed)
            {
                transform_data.position = GetScreenToWorld2D(screen_position, camera_data);
            }
        }
    }

   protected:
    entt::registry& registry;
};

#endif // BASE_PROCESSORS_HPP
