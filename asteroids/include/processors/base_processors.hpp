#ifndef BASE_PROCESSORS_HPP
#define BASE_PROCESSORS_HPP

#include <raylib.h>

#include <entt/entt.hpp>

#include "components/base.hpp"

struct cleanup_process : entt::process<cleanup_process, std::uint32_t>
{
    using delta_type = std::uint32_t;

    cleanup_process(entt::registry& registry) :
        registry(registry) {}

    void update(delta_type delta_time, void*)
    {
        auto cleanup_view = registry.view<entt::tag<kill_tag>>();

        for (auto [entity] : cleanup_view.each())
        {
            registry.destroy(entity);
        }
    }

   protected:
    entt::registry& registry;
};

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
                if (lifetime_data.on_end != nullptr)
                {
                    lifetime_data.on_end(registry);
                }

                registry.emplace<entt::tag<kill_tag>>(entity);
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
        const int border_width = 50;

        auto boundable_view = registry.view<transform>();
        auto camera_view    = registry.view<Camera2D>();

        auto camera_entity = camera_view.front();
        auto camera_data   = camera_view.get<Camera2D>(camera_entity);

        for (auto [entity, transform_data] : boundable_view.each())
        {
            auto screen_position = GetWorldToScreen2D(transform_data.position, camera_data);
            bool has_changed     = false;

            if (screen_position.x <= 0 - border_width)
            {
                screen_position.x = GetScreenWidth() + border_width;
                has_changed       = true;
            } else if (screen_position.x >= GetScreenWidth() + border_width)
            {
                screen_position.x = 0 - border_width;
                has_changed       = true;
            }

            if (screen_position.y <= 0 - border_width)
            {
                screen_position.y = GetScreenHeight() + border_width;
                has_changed       = true;
            } else if (screen_position.y >= GetScreenHeight() + border_width)
            {
                screen_position.y = 0 - border_width;
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
