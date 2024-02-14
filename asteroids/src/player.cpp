#include "components/player.hpp"

#include <raylib.h>

#include <components/base.hpp>
#include <iostream>

#include "components/asteroid.hpp"
#include "components/physics.hpp"
#include "components/render.hpp"
#include "raylib.h"
#include "raymath.h"

void on_player_collision(entt::registry& registry, entt::entity player_entity, entt::entity other_entity)
{
    std::cout << "Player collision" << std::endl;
    if (!registry.all_of<asteroid>(other_entity))
        return;

    auto player_physics = registry.get<physics>(player_entity);
    auto asteroid_data  = registry.get<asteroid>(other_entity);

    if (registry.valid(other_entity))
    {
        asteroid_data.on_asteroid_death(registry, other_entity, Vector2Normalize(player_physics.velocity), asteroid_data.level - 1);
    }

    if (registry.valid(player_entity))
    {
        auto& player_data = registry.get<Player>(player_entity);
        player_data.lives -= 1;

        if (player_data.lives <= 0)
        {
            registry.destroy(player_entity);
            return;
        }
    }
}

entt::entity create_player(entt::registry& registry, uint8_t id)
{
    entt::entity entity = registry.create();

    int screenWidth  = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    circle_collider player_collider;
    player_collider.radius = 10;
    player_collider.on_collision.connect<&on_player_collision>();

    registry.emplace<Player>(entity, Player{id, 0, 3});
    registry.emplace<transform>(
        entity,
        transform{
            // Vector2{100, 100},
            Vector2{screenWidth * 0.5f, screenHeight * 0.5f},
            Vector2{0, 1}, 0});
    registry.emplace<physics>(entity, physics{Vector2{0, 0}, 0, 0.005f, Vector2{0, 0}, Vector2{0, 0}});
    registry.emplace<circle_collider>(entity, player_collider);

    float scale       = player_collider.radius * 2 / 96.0f;
    Texture tilesheet = LoadTexture("asteroids/resources/simpleSpace_tilesheet.png");
    registry.emplace<sprite_render>(entity, sprite_render{tilesheet, Rectangle{528, 16, 96, 96}, scale});

    // add_render_data(registry, entity, WHITE);

    return entity;
}

void on_bullet_collision(entt::registry& registry, entt::entity bullet_entity, entt::entity other_entity)
{
    std::cout << "Bullet collision" << std::endl;
    if (!registry.all_of<asteroid>(other_entity))
        return;

    auto bullet_physics = registry.get<physics>(bullet_entity);
    auto asteroid_data  = registry.get<asteroid>(other_entity);

    if (registry.valid(other_entity))
    {
        asteroid_data.on_asteroid_death(registry, other_entity, Vector2Normalize(bullet_physics.velocity), asteroid_data.level - 1);
    }

    if (registry.valid(bullet_entity))
    {
        registry.destroy(bullet_entity);
    }
}

static std::unique_ptr<float> radius_ptr = std::make_unique<float>(1.5f);
entt::entity spawn_bullet(entt::registry& registry, Vector2 position, Vector2 velocity)
{
    entt::entity entity = registry.create();
    float angle         = atan2(velocity.y, velocity.x) * RAD2DEG;

    shape_render render_data;
    render_data.color = RED;
    render_data.shape = render_shape_type::CIRCLE;
    render_data.data  = static_cast<void*>(radius_ptr.get());

    registry.emplace<transform>(entity, transform{position, Vector2{0, 1}, angle});
    registry.emplace<physics>(entity, physics{velocity, 0, 0.0f, Vector2{0, 0}, Vector2{0, 0}});
    registry.emplace<shape_render>(entity, render_data);
    registry.emplace<lifetime>(entity, lifetime{2.5f, 0});

    circle_collider bullet_collider;
    bullet_collider.radius = 1.5f;
    bullet_collider.on_collision.connect<&on_bullet_collision>();
    registry.emplace<circle_collider>(entity, bullet_collider);

    return entity;
}
