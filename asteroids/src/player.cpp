#include <iostream>
#include <player.hpp>

#include "components/asteroid.hpp"
#include "components/base.hpp"
#include "components/physics.hpp"
#include "components/render.hpp"
#include "math.hpp"
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

	float scale = player_collider.radius * 2 / 96.0f;
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

void on_player_score(entt::registry& registry, entt::entity asteroid_entity, Vector2 normalizedDirection, int8_t level)
{
    uint32_t score = 0;
    switch (level)
    {
        case 0:
            score = 100;
            break;
        case 1:
            score = 50;
            break;
        case 2:
            score = 20;
            break;
        default:
            score = 200;
            break;
    }

    auto player_view   = registry.view<Player>();
    auto player_entity = player_view.front();

    if (!registry.valid(player_entity))
        return;

    auto& player_data = player_view.get<Player>(player_entity);

    player_data.score += score;
}

void on_asteroid_break(entt::registry& registry, entt::entity asteroid_entity, Vector2 normalizedDirection, int8_t level)
{
    on_player_score(registry, asteroid_entity, normalizedDirection, level);

    if (level <= 0)
    {
        registry.destroy(asteroid_entity);
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
    registry.emplace<lifetime>(entity, lifetime{2.5f, 0});

    circle_collider bullet_collider;
    bullet_collider.radius = 1.5f;
    bullet_collider.on_collision.connect<&on_bullet_collision>();
    registry.emplace<circle_collider>(entity, bullet_collider);

    return entity;
}

void spawn_random_asteroid_distribution(entt::registry& registry, int count)
{
    for (int i = 0; i < count; i++)
    {
        float angle = GetRandomValue(0, 360);
        float x     = GetRandomValue(20, GetScreenWidth() - 20);
        float y     = GetRandomValue(20, GetScreenHeight() - 20);
        float speed = GetRandomValue(100, 200);

        Vector2 position = {x, y};
        Vector2 velocity = Vector2Normalize(Vector2{cosf(angle * DEG2RAD), sinf(angle * DEG2RAD)}) * speed;

        spawn_asteroid(registry, position, velocity, 3);
    }
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

    circle_collider asteroid_collider;
    asteroid_collider.radius = *radius;
    registry.emplace<circle_collider>(entity, asteroid_collider);

    return entity;
}
