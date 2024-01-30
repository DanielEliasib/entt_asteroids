#include <iostream>
#include <player.hpp>

#include "components/asteroid.hpp"
#include "components/base.hpp"
#include "components/physics.hpp"
#include "components/render.hpp"
#include "math.hpp"
#include "raymath.h"

entt::entity create_player(entt::registry& registry, uint8_t id)
{
    entt::entity entity = registry.create();
    registry.emplace<Player>(entity, Player{id});
    registry.emplace<transform>(entity, transform{Vector2{400, 300}, Vector2{0, 1}, 0});
    registry.emplace<physics>(entity, physics{Vector2{0, 0}, 0, 0.005f, Vector2{0, 0}, Vector2{0, 0}});

    add_render_data(registry, entity, WHITE);

    return entity;
}

void on_bullet_collision(entt::registry& registry, entt::entity bullet_entity, entt::entity other_entity)
{
    std::cout << "Bullet collision" << std::endl;
    if (!registry.all_of<asteroid>(other_entity))
        return;

    auto bullet_physics = registry.get<physics>(bullet_entity);

    if (registry.valid(bullet_entity))
    {
        registry.destroy(bullet_entity);
    }

    std::cout << "Bullet collision with asteroid" << std::endl;

    auto asteroid_data = registry.get<asteroid>(other_entity);
    asteroid_data.on_asteroid_death(registry, other_entity, Vector2Normalize(bullet_physics.velocity), asteroid_data.level - 1);
}

void on_asteroid_break(entt::registry& registry, entt::entity asteroid_entity, Vector2 normalizedDirection, int8_t level)
{
    std::cout << "Asteroid break" << std::endl;
    if (level <= 0)
    {
        std::cout << "Asteroid break level 0" << std::endl;

        if (registry.valid(asteroid_entity))
        {
            registry.destroy(asteroid_entity);
        }

        return;
    }

    auto generate_asteroid = [&registry, &normalizedDirection, &level](Vector2 position, Vector2 velocity) {
        float angle          = GetRandomValue(-135, 135);
        auto break_direction = Vector2Rotate(normalizedDirection, angle * DEG2RAD) * 350.0f;

        velocity = velocity + break_direction;

        spawn_asteroid(registry, position, velocity, level);
    };

    auto asteroid_data  = registry.get<asteroid>(asteroid_entity);
    auto transform_data = registry.get<transform>(asteroid_entity);
    auto physics_data   = registry.get<physics>(asteroid_entity);

    std::cout << "Asteroid break level " << static_cast<int>(level) << std::endl;
    registry.destroy(asteroid_entity);

    generate_asteroid(transform_data.position, physics_data.velocity);
    generate_asteroid(transform_data.position, physics_data.velocity);
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
    registry.emplace<lifetime>(entity, lifetime{1500});

    circle_collider bullet_collider;
    bullet_collider.radius = 1.5f;
    bullet_collider.on_collision.connect<&on_bullet_collision>();
    registry.emplace<circle_collider>(entity, bullet_collider);

    return entity;
}

static std::unique_ptr<float> a_radius_2_ptr = std::make_unique<float>(50.0f);
static std::unique_ptr<float> a_radius_1_ptr = std::make_unique<float>(25.0f);
static std::unique_ptr<float> a_radius_0_ptr = std::make_unique<float>(10.0f);
entt::entity spawn_asteroid(entt::registry& registry, Vector2 position, Vector2 velocity, int8_t level)
{
    auto level_radius = [](int8_t level) {
        if (level >= 3)
        {
            return a_radius_2_ptr.get();
        } else if (level >= 2)
        {
            return a_radius_1_ptr.get();
        } else
        {
            return a_radius_0_ptr.get();
        }
    };

    if (level <= 0)
    {
        std::cout << "Spawn invalid asteroid" << std::endl;
        return entt::null;
    }

    entt::entity entity = registry.create();
    float angle         = atan2(velocity.y, velocity.x) * RAD2DEG;
    auto radius         = level_radius(level);

    shape_render render_data;
    render_data.color = WHITE;
    render_data.shape = render_shape_type::CIRCLE;
    render_data.data  = static_cast<void*>(radius);

    asteroid asteroid_data;
    asteroid_data.level = level;
    asteroid_data.on_asteroid_death.connect<&on_asteroid_break>();

    registry.emplace<transform>(entity, transform{position, Vector2{0, 1}, angle});
    registry.emplace<physics>(entity, physics{velocity, 0, 0.0f, Vector2{0, 0}, Vector2{0, 0}});
    registry.emplace<shape_render>(entity, render_data);
    registry.emplace<asteroid>(entity, asteroid_data);
    registry.emplace<lifetime>(entity, lifetime{5000});

    circle_collider asteroid_collider;
    asteroid_collider.radius = *radius;
    registry.emplace<circle_collider>(entity, asteroid_collider);

    return entity;
}
