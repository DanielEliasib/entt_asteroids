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

    auto player_physics   = registry.get<physics>(player_entity);
    auto player_transform = registry.get<transform>(player_entity);
    auto asteroid_data    = registry.get<asteroid>(other_entity);

    if (registry.valid(other_entity))
    {
        asteroid_data.on_asteroid_death(registry, other_entity, Vector2Normalize(player_physics.velocity), asteroid_data.level - 1);
    }

    if (registry.valid(player_entity))
    {
        auto player_data_entry = registry.view<Player>().front();

        auto& player_data = registry.get<Player>(player_data_entry);
        player_data.lives -= 1;

        Player player_data_copy = player_data;

        registry.destroy(player_entity);
        spawn_explosion(registry, player_transform.position, 3);

        if (player_data.lives <= 0)
        {
            return;
        }

        lifetime restore_cooldown;
        restore_cooldown.lifetime = 3;
        restore_cooldown.on_end   = [](entt::registry& registry) {
            create_player(registry, 0);
        };

        auto restore_player_entity = registry.create();
        registry.emplace<lifetime>(restore_player_entity, restore_cooldown);
    }
}

entt::entity create_player(entt::registry& registry, uint8_t id)
{
    static const std::uint32_t texture_tag = static_cast<std::uint32_t>(GAME_TEXTURES::MAINTEXTURE);
    entt::entity entity                    = registry.create();

    int screenWidth  = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    circle_collider player_collider;
    player_collider.radius = 10;
    player_collider.on_collision.connect<&on_player_collision>();

    // registry.emplace<Player>(entity, player_data);
    registry.emplace<transform>(
        entity,
        transform{
            // Vector2{100, 100},
            Vector2{screenWidth * 0.5f, screenHeight * 0.5f},
            Vector2{0, 1}, 0});
    registry.emplace<physics>(entity, physics{Vector2{0, 0}, 0, 0.005f, Vector2{0, 0}, Vector2{0, 0}});
    registry.emplace<circle_collider>(entity, player_collider);
    registry.emplace<entt::tag<player_tag>>(entity);

	// TODO: CLEAN
    bool player_exists = false;
    auto player_view   = registry.view<Player>();
    for (auto player_entity : player_view)
    {
        auto& player_data = player_view.get<Player>(player_entity);

        if (player_data.id == id)
        {
            player_exists = true;
        }
    }

    if (!player_exists)
    {
        entt::entity player_data_entity = registry.create();
        registry.emplace<Player>(player_data_entity, Player{id, 0, 3});
    }

    float scale = player_collider.radius * 2 / 96.0f;

    auto texture_entity = registry.view<Texture2D, entt::tag<texture_tag>>().front();
    Texture2D tilesheet = registry.get<Texture2D>(texture_entity);

    registry.emplace<sprite_render>(entity, sprite_render{tilesheet, Rectangle{528, 16, 96, 96}, scale});

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
    static const std::uint32_t texture_tag = static_cast<std::uint32_t>(GAME_TEXTURES::MAINTEXTURE);
    entt::entity entity                    = registry.create();
    float angle                            = atan2(velocity.y, velocity.x) * RAD2DEG;

    shape_render render_data;
    render_data.color = RED;
    render_data.shape = render_shape_type::CIRCLE;
    render_data.data  = static_cast<void*>(radius_ptr.get());

    registry.emplace<transform>(entity, transform{position, Vector2{0, 1}, angle});
    registry.emplace<physics>(entity, physics{velocity, 0, 0.0f, Vector2{0, 0}, Vector2{0, 0}});
    // registry.emplace<shape_render>(entity, render_data);
    registry.emplace<lifetime>(entity, lifetime{2.5f, 0});

    circle_collider bullet_collider;
    bullet_collider.radius = 2.5f;
    bullet_collider.on_collision.connect<&on_bullet_collision>();
    registry.emplace<circle_collider>(entity, bullet_collider);

    float scale = bullet_collider.radius * 2 / 32.0f;

    auto texture_entity = registry.view<Texture2D, entt::tag<texture_tag>>().front();
    Texture2D tilesheet = registry.get<Texture2D>(texture_entity);

    registry.emplace<sprite_render>(entity, sprite_render{tilesheet, Rectangle{560, 432, 32, 32}, scale, RED});

    return entity;
}

entt::entity spawn_explosion(entt::registry& registry, Vector2 position, float scale)
{
    static const std::uint32_t texture_tag    = static_cast<std::uint32_t>(GAME_TEXTURES::PLANETEXTURE);
    static std::array<sprite_frame, 5> frames = {
        {Rectangle{68, 0, 16, 16},
         Rectangle{85, 0, 16, 16},
         Rectangle{102, 0, 16, 16},
         Rectangle{119, 0, 16, 16},
         Rectangle{136, 0, 16, 16}}};

    auto texture_entity = registry.view<Texture2D, entt::tag<texture_tag>>().front();
    Texture2D tilesheet = registry.get<Texture2D>(texture_entity);

    entt::entity entity = registry.create();

    sprite_sequence explosion_sequence = {
        .frames        = frames.data(),
        .first_frame   = frames.data(),
        .loop          = false,
        .update        = true,
        .frame_count   = 5,
        .current_frame = 0,
        .frame_time    = 0.2f,
    };

    registry.emplace<sprite_render>(entity, sprite_render{tilesheet, frames[0].source, scale, WHITE});
    registry.emplace<sprite_sequence>(entity, explosion_sequence);
    registry.emplace<transform>(entity, transform{position, Vector2{0, 1}, 0});
    registry.emplace<lifetime>(entity, lifetime{0.2f * 5, 0});

    return entity;
}
