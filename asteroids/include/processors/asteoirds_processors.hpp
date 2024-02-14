#ifndef ASTEROIDS_PROCESSORS_HPP
#define ASTEROIDS_PROCESSORS_HPP

#include <raylib.h>

#include <entt/entt.hpp>


struct asteroid_spawn_process : entt::process<asteroid_spawn_process, std::uint32_t>
{
    using delta_type = std::uint32_t;

    asteroid_spawn_process(entt::registry& registry) :
        registry(registry) {}

    void update(delta_type delta_time, void*)
    {
        // cooldown -= delta_time;
        // if (cooldown > 0)
        // {
        //     return;
        // }
        //
        // std::cout << "Spawn asteroid" << std::endl;
        //
        // Camera2D camera = registry.get<Camera2D>(registry.view<Camera2D>().front());
        //
        // auto player_view           = registry.view<Player, transform>();
        // auto player_entity         = player_view.front();
        // auto player_transform_data = player_view.get<transform>(player_entity);
        //
        // auto screen_width  = GetScreenWidth() * 2;
        // auto screen_height = GetScreenHeight() * 2;
        //
        // Vector2 position = Vector2{0, 0};
        //
        // bool x_or_y = GetRandomValue(0, 1);
        // if (x_or_y)
        // {
        //     bool top_down = GetRandomValue(0, 1);
        //
        //     position.x = GetRandomValue(0, screen_width) - screen_width / 2;
        //     position.y = top_down ? -100 : GetScreenHeight() + 100;
        // } else
        // {
        //     bool left_right = GetRandomValue(0, 1);
        //
        //     position.x = left_right ? -100 : GetScreenWidth() + 100;
        //     position.y = GetRandomValue(0, screen_height) - screen_height / 2;
        // }
        //
        // position = GetScreenToWorld2D(position, camera);
        //
        // Vector2 direction = player_transform_data.position - position;
        // direction         = Vector2Normalize(direction);
        //
        // spawn_asteroid(registry, position, direction * 250, 3);
        //
        // cooldown = 1500;
    }

   protected:
    entt::registry& registry;
    int cooldown = 0.0f;

    std::uint32_t cooldown_max = 1000.0f;
    std::uint32_t cooldown_min = 100.0f;
};

#endif // ASTEROIDS_PROCESSORS_HPP
