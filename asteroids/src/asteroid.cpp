#include "components/asteroid.hpp"

#include <raylib.h>

#include <components/render.hpp>
#include <iostream>

#include "components/base.hpp"
#include "components/physics.hpp"
#include "components/player.hpp"
#include "math.hpp"
#include "teams.hpp"

void spawn_random_asteroid_distribution(entt::registry& registry, int count)
{
    const float screenWidth  = GetScreenWidth();
    const float screenHeight = GetScreenHeight();

    const Rectangle top_rect    = {-40, -40, screenWidth + 40.0f, 40};
    const Rectangle bottom_rect = {-40, screenHeight, screenWidth + 40.0f, 40};
    const Rectangle left_rect   = {-40, -40, 40, screenHeight + 40.0f};
    const Rectangle right_rect  = {screenWidth, -40, 40, screenHeight + 40.0f};

    const std::array<Rectangle, 4> rects = {top_rect, bottom_rect, left_rect, right_rect};

    for (int i = 0; i < count; i++)
    {
        int random            = GetRandomValue(0, 3);
        const Rectangle& rect = rects[random];

        float x = GetRandomValue(rect.x, rect.x + rect.width);
        float y = GetRandomValue(rect.y, rect.y + rect.height);

        float angle = GetRandomValue(15, 345);
        float speed = GetRandomValue(100, 200);

        Vector2 position = {x, y};
        Vector2 velocity = Vector2Normalize(Vector2{cosf(angle * DEG2RAD), sinf(angle * DEG2RAD)}) * speed;

        spawn_asteroid(registry, position, velocity, 3);
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

void on_asteroid_break_by_bullet(entt::registry& registry, entt::entity bullet_entity, entt::entity asteroid_entity)
{
    if (!registry.all_of<physics>(bullet_entity) || !registry.all_of<asteroid>(asteroid_entity))
        return;

    auto bullet_physics         = registry.get<physics>(bullet_entity);
    Vector2 normalizedDirection = Vector2Normalize(bullet_physics.velocity);

    auto asteroid_data = registry.get<asteroid>(asteroid_entity);
    int8_t level       = asteroid_data.level - 1;

    asteroid_data.on_asteroid_death(registry, asteroid_entity, normalizedDirection, level);
}

void on_asteroid_break(entt::registry& registry, entt::entity asteroid_entity, Vector2 normalizedDirection, int8_t level)
{
    if (!registry.all_of<asteroid>(asteroid_entity))
        return;

    auto asteroid_data = registry.get<asteroid>(asteroid_entity);

    on_player_score(registry, asteroid_entity, normalizedDirection, level);

    if (level <= 0)
    {
        registry.emplace<entt::tag<kill_tag>>(asteroid_entity);
        return;
    }

    auto generate_asteroid = [&registry, &normalizedDirection, &level](Vector2 position, Vector2 velocity) {
        float angle          = GetRandomValue(-135, 135);
        auto break_direction = Vector2Rotate(normalizedDirection, angle * DEG2RAD) * 350.0f;

        velocity = velocity + break_direction;

        spawn_asteroid(registry, position, velocity, level);
    };

    auto transform_data = registry.get<transform>(asteroid_entity);
    auto physics_data   = registry.get<physics>(asteroid_entity);

    std::cout << "Asteroid break level " << static_cast<int>(level) << std::endl;
    registry.emplace<entt::tag<kill_tag>>(asteroid_entity);

    generate_asteroid(transform_data.position, physics_data.velocity);
    generate_asteroid(transform_data.position, physics_data.velocity);
}

void on_asteroid_collision(entt::registry& registry, entt::entity asteroid_entity, entt::entity other_entity)
{
    std::cout << "Asteroid collision" << std::endl;
    if (!registry.all_of<asteroid_collision_response>(other_entity))
        return;

    if (!registry.all_of<team>(other_entity) || !registry.all_of<team>(asteroid_entity))
        return;

    auto asteroid_responder_data = registry.get<asteroid_collision_response>(other_entity);
    auto asteroid_team           = registry.get<team>(asteroid_entity);

    auto other_team = registry.get<team>(other_entity);

    if (asteroid_team == other_team)
        return;

    if (registry.valid(other_entity))
    {
        asteroid_responder_data.on_collision(registry, asteroid_entity, other_entity);
    }
}

static std::unique_ptr<float> a_radius_2_ptr = std::make_unique<float>(50.0f);
static std::unique_ptr<float> a_radius_1_ptr = std::make_unique<float>(25.0f);
static std::unique_ptr<float> a_radius_0_ptr = std::make_unique<float>(10.0f);
entt::entity spawn_asteroid(entt::registry& registry, Vector2 position, Vector2 velocity, int8_t level)
{
    static const auto make_rectangle_source = [](int8_t level, float sprite_size) {
        if (level >= 3)
        {
            return Rectangle{16, 528, sprite_size, sprite_size};
        } else
        {
            return Rectangle{160, 544, sprite_size, sprite_size};
        }
    };

    static const auto level_radius = [](int8_t level) {
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

    static const auto random_color = []() {
        static const std::array<Color, 5> colors = {
            Color{224, 229, 231, 255},
            Color{220, 222, 227, 255},
            Color{233, 238, 240, 255},
        };

        return colors[GetRandomValue(0, 2)];
    };

    static const std::uint32_t texture_tag = static_cast<std::uint32_t>(GAME_TEXTURES::MAINTEXTURE);

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
    registry.emplace<asteroid>(entity, asteroid_data);

    circle_collider asteroid_collider;
    asteroid_collider.radius = *radius;
    asteroid_collider.on_collision.connect<&on_asteroid_collision>();
    registry.emplace<circle_collider>(entity, asteroid_collider);

    float sprite_size = level < 3 ? 64 : 96;
    float scale       = asteroid_collider.radius * 2 / sprite_size;
    Rectangle source  = make_rectangle_source(level, sprite_size);

    auto texture_entity = registry.view<Texture2D, entt::tag<texture_tag>>().front();
    Texture2D tilesheet = registry.get<Texture2D>(texture_entity);

    bullet_collision_response bullet_responder;
    bullet_responder.on_collision.connect<&on_asteroid_break_by_bullet>();
    registry.emplace<bullet_collision_response>(entity, bullet_responder);

    registry.emplace<sprite_render>(entity, sprite_render{tilesheet, source, scale, random_color()});
    registry.emplace<team>(entity, team::ENEMY);

    return entity;
}
