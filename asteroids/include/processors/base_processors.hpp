#ifndef BASE_PROCESSORS_HPP
#define BASE_PROCESSORS_HPP

#include <entt/entt.hpp>

#include "components/base.hpp"
#include "player.hpp"
#include "raymath.h"

struct lifetime_process : entt::process<lifetime_process, std::uint32_t>
{
    using delta_type = std::uint32_t;

    lifetime_process(entt::registry& registry) :
        registry(registry) {}

    void update(delta_type delta_time, void*)
    {
        auto player_view   = registry.view<Player, transform>();
        auto lifetime_view = registry.view<lifetime, transform>();

        auto player_entity = player_view.front();
        if (player_entity == entt::null)
        {
            return;
        }

        auto player_transform_data = player_view.get<transform>(player_entity);

        for (auto [entity, lifetime_data, transform_data] : lifetime_view.each())
        {
            if (Vector2DistanceSqr(player_transform_data.position, transform_data.position) > lifetime_data.radius * lifetime_data.radius)
            {
                if (!registry.valid(entity))
                    continue;

                registry.destroy(entity);
            }
        }
    }

   protected:
    entt::registry& registry;
};

#endif // BASE_PROCESSORS_HPP
