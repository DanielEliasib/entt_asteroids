#include "components/player.hpp"

#include <raylib.h>

#include <components/base.hpp>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <math.hpp>
#include <memory>
#include <sstream>
#include <vector>

#include "components/physics.hpp"
#include "components/render.hpp"
#include "raylib.h"
#include "scenes/scene_management.hpp"

void on_player_explosion(entt::registry& registry, entt::entity player_entity, entt::entity other_entity)
{
    auto trail_view = registry.view<entt::tag<player_trail_tag>>();

    for (auto trail_entity : trail_view)
    {
        registry.emplace<entt::tag<kill_tag>>(trail_entity);
    }

    auto player_physics   = registry.get<physics>(player_entity);
    auto player_transform = registry.get<transform>(player_entity);

    if (registry.valid(player_entity))
    {
        auto player_data_entry = registry.view<Player>().front();

        auto& player_data = registry.get<Player>(player_data_entry);
        player_data.lives -= 1;

        Player player_data_copy = player_data;

        registry.emplace<entt::tag<kill_tag>>(player_entity);
        spawn_explosion(registry, player_transform.position, 3);

        if (player_data.lives <= 0)
        {
            spawn_game_over(registry);

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

void on_player_collision_with_object(entt::registry& registry, entt::entity other_entity, entt::entity player_entity)
{
    on_player_explosion(registry, player_entity, other_entity);
}

entt::entity create_player(entt::registry& registry, uint8_t id)
{
    static const std::uint32_t texture_tag = static_cast<std::uint32_t>(GAME_TEXTURES::MAINTEXTURE);
    entt::entity entity                    = registry.create();

    int screenWidth  = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    circle_collider player_collider;
    player_collider.radius = 10;

    registry.emplace<transform>(
        entity,
        transform{
            Vector2{screenWidth * 0.5f, screenHeight * 0.5f},
            0});
    registry.emplace<physics>(entity, physics{Vector2{0, 0}, 0, 0.005f, Vector2{0, 0}, Vector2{0, 0}});
    registry.emplace<circle_collider>(entity, player_collider);
    registry.emplace<entt::tag<player_tag>>(entity);
    registry.emplace<team>(entity, team::PLAYER);

    bullet_collision_response collision_response_to_bullet;
    collision_response_to_bullet.on_collision.connect<&on_player_collision_with_object>();
    registry.emplace<bullet_collision_response>(entity, collision_response_to_bullet);

    asteroid_collision_response collision_response_to_asteroid;
    collision_response_to_asteroid.on_collision.connect<&on_player_collision_with_object>();
    registry.emplace<asteroid_collision_response>(entity, collision_response_to_asteroid);

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

    float scale  = player_collider.radius * 2 / 96.0f;
    float scale2 = player_collider.radius * 2 / 64.0f;

    auto texture_entity = registry.view<Texture2D, entt::tag<texture_tag>>().front();
    Texture2D tilesheet = registry.get<Texture2D>(texture_entity);

    registry.emplace<sprite_render>(entity, sprite_render{tilesheet, Rectangle{528, 16, 96, 96}, scale});
    // registry.emplace<sprite_render>(entity, sprite_render{tilesheet, Rectangle{800, 640, 64, 124}, scale2});

    auto trail_entity = registry.create();
    registry.emplace<transform>(
        trail_entity,
        transform{
            Vector2{screenWidth * 0.5f, screenHeight * 0.5f},
            0});

    registry.emplace<entt::tag<player_trail_tag>>(trail_entity);
    registry.emplace<sprite_render>(trail_entity,
                                    sprite_render{
                                        tilesheet,
                                        Rectangle{928, 640, 64, 124},
                                        // Rectangle{800, 640, 64, 124},
                                        scale2,
                                        WHITE,
                                        Vector2{0, -(150 / 2) * scale2}});

    return entity;
}

void on_bullet_collision(entt::registry& registry, entt::entity bullet_entity, entt::entity other_entity)
{
    if (!registry.all_of<bullet_collision_response>(other_entity))
        return;

    if (!registry.all_of<team>(other_entity) || !registry.all_of<team>(bullet_entity))
        return;

    auto bullet_physics        = registry.get<physics>(bullet_entity);
    auto bullet_responder_data = registry.get<bullet_collision_response>(other_entity);
    auto bullet_team           = registry.get<team>(bullet_entity);

    auto other_team = registry.get<team>(other_entity);

    if (bullet_team == other_team)
        return;

    if (registry.valid(other_entity))
    {
        bullet_responder_data.on_collision(registry, bullet_entity, other_entity);
    }

    if (registry.valid(bullet_entity))
    {
        registry.emplace<entt::tag<kill_tag>>(bullet_entity);
    }
}

static std::unique_ptr<float> radius_ptr = std::make_unique<float>(1.5f);

entt::entity spawn_bullet(entt::registry& registry, Vector2 position, Vector2 velocity, const team& bullet_team)
{
    static const std::uint32_t blue_texture_tag = static_cast<std::uint32_t>(GAME_TEXTURES::BULLETTEXTURE_BLUE);
    static const std::uint32_t red_texture_tag  = static_cast<std::uint32_t>(GAME_TEXTURES::BULLETTEXTURE_RED);

    static std::shared_ptr<std::vector<sprite_frame>> frames =
        std::make_shared<std::vector<sprite_frame>>(std::initializer_list<sprite_frame>{{Rectangle{256, 96, 16, 16}},
                                                                                        {Rectangle{272, 96, 16, 16}},
                                                                                        {Rectangle{288, 96, 16, 16}},
                                                                                        {Rectangle{304, 96, 16, 16}}});

    entt::entity entity = registry.create();
    float angle         = atan2(velocity.y, velocity.x) * RAD2DEG;

    registry.emplace<transform>(entity, transform{position, angle});
    registry.emplace<physics>(entity, physics{velocity, 0, 0.0f, Vector2{0, 0}, Vector2{0, 0}});
    // registry.emplace<shape_render>(entity, render_data);
    registry.emplace<lifetime>(entity, lifetime{2.5f, 0});

    circle_collider bullet_collider;
    bullet_collider.radius = 3.5f;
    bullet_collider.on_collision.connect<&on_bullet_collision>();
    registry.emplace<circle_collider>(entity, bullet_collider);

    float scale = bullet_collider.radius * 2 / 16.0f;

    entt::entity texture_entity = entt::null;
    if (bullet_team == team::PLAYER)
    {
        texture_entity = registry.view<Texture2D, entt::tag<blue_texture_tag>>().front();
    } else
    {
        texture_entity = registry.view<Texture2D, entt::tag<red_texture_tag>>().front();
    }

    Texture2D tilesheet = registry.get<Texture2D>(texture_entity);

    sprite_sequence explosion_sequence = {
        .frames              = frames,
        .loop                = true,
        .update              = true,
        .current_frame_index = 0,
        .frame_time          = 0.2f,
    };
    registry.emplace<sprite_render>(entity,
                                    sprite_render{
                                        tilesheet,
                                        frames->at(0).source,
                                        scale,
                                        WHITE});
    registry.emplace<sprite_sequence>(entity, explosion_sequence);

    registry.emplace<team>(entity, bullet_team);

    return entity;
}

entt::entity spawn_explosion(entt::registry& registry, Vector2 position, float scale)
{
    static const std::uint32_t texture_tag = static_cast<std::uint32_t>(GAME_TEXTURES::PLANETEXTURE);

    static std::shared_ptr<std::vector<sprite_frame>> frames =
        std::make_shared<std::vector<sprite_frame>>(std::initializer_list<sprite_frame>{{Rectangle{68, 0, 16, 16}},
                                                                                        {Rectangle{85, 0, 16, 16}},
                                                                                        {Rectangle{102, 0, 16, 16}},
                                                                                        {Rectangle{119, 0, 16, 16}},
                                                                                        {Rectangle{136, 0, 16, 16}}});

    auto texture_entity = registry.view<Texture2D, entt::tag<texture_tag>>().front();
    Texture2D tilesheet = registry.get<Texture2D>(texture_entity);

    entt::entity entity = registry.create();

    sprite_sequence explosion_sequence = {
        .frames              = frames,
        .loop                = false,
        .update              = true,
        .current_frame_index = 0,
        .frame_time          = 0.2f,
    };

    registry.emplace<sprite_render>(entity, sprite_render{tilesheet, frames->at(0).source, scale, WHITE});
    registry.emplace<sprite_sequence>(entity, explosion_sequence);
    registry.emplace<transform>(entity, transform{position, 0});
    registry.emplace<lifetime>(entity, lifetime{0.2f * 5, 0});

    return entity;
}

void spawn_game_ui(entt::registry& registry)
{
    auto title_text_entity = registry.create();

    auto make_dynamic_text = [&registry](std::function<const char*(entt::registry&)> text, int font_size, Vector2 position, Color color) {
        auto text_entity = registry.create();

        dynamic_text_render text_component;
        text_component.font          = GetFontDefault();
        text_component.text_function = text;
        text_component.font_size     = font_size;
        text_component.color         = color;
        text_component.center        = true;

        transform text_transform;
        text_transform.position = position;
        text_transform.rotation = 0.0f;

        registry.emplace<transform>(text_entity, text_transform);
        registry.emplace<dynamic_text_render>(text_entity, text_component);
    };

    auto score_text = [](entt::registry& registry) -> const char* {
        auto player_entity = registry.view<Player>().front();

        if (!registry.valid(player_entity))
        {
            return "";
        }

        auto player_data = registry.get<Player>(player_entity);

        auto score = TextFormat("%05i", player_data.score);
        return score;
    };

    auto lives_text = [](entt::registry& registry) -> const char* {
        auto player_entity = registry.view<Player>().front();

        if (!registry.valid(player_entity))
        {
            return "";
        }

        auto player_data = registry.get<Player>(player_entity);

        std::ostringstream ss;

        for (int i = 0; i < player_data.lives; i++)
        {
            ss << "X";
        }

        ss << std::setw(3)
           << std::setfill(' ');

        return TextFormat("%s", ss.str().c_str());
    };

    float screenWidth = GetScreenWidth();

    Vector2 position = Vector2{screenWidth / 2.0f,
                               10};

    make_dynamic_text(score_text, 30, position, RAYWHITE);

    position.y = 40;
    make_dynamic_text(lives_text, 30, position, RAYWHITE);
}

void spawn_game_over(entt::registry& registry)
{
    auto make_text = [&registry](const char* text, int font_size, Vector2 position, Color color) {
        auto text_entity = registry.create();

        text_render text_component;
        text_component.font      = GetFontDefault();
        text_component.text      = text;
        text_component.font_size = font_size;
        text_component.color     = color;

        int textWidth = MeasureText(text_component.text, text_component.font_size);

        transform text_transform;
        text_transform.position = position;
        text_transform.rotation = 0.0f;

        registry.emplace<transform>(text_entity, text_transform);
        registry.emplace<text_render>(text_entity, text_component);
    };

    float screenWidth  = GetScreenWidth();
    float screenHeight = GetScreenHeight();

    const char* title_text = "GAME OVER";

    int fontSize  = 50;
    int textWidth = MeasureText(title_text, fontSize);

    Vector2 position = Vector2{(screenWidth - textWidth) / 2.0f,
                               screenHeight / 2.0f - 50.0f};

    make_text(title_text, fontSize, position, RED);
    make_text(title_text, fontSize, position + Vector2{5, 5}, BLACK);

    const char* subtitle_text = "Press ESC to close";

    fontSize  = 30;
    textWidth = MeasureText(subtitle_text, fontSize);

    position = Vector2{(screenWidth - textWidth) / 2.0f,
                       screenHeight / 2.0f + 10.0f};

    make_text(subtitle_text, fontSize, position, RAYWHITE);
    make_text(subtitle_text, fontSize, position + Vector2{5, 5}, BLACK);

    const char* restart_text = "Press SPACE to restart";

    fontSize  = 20;
    textWidth = MeasureText(restart_text, fontSize);

    position = Vector2{(screenWidth - textWidth) / 2.0f,
                       screenHeight / 2.0f + 50.0f};

    make_text(restart_text, fontSize, position, text_color);
    make_text(restart_text, fontSize, position + Vector2{5, 5}, BLACK);

    auto player_view   = registry.view<Player>();
    auto player_entity = player_view.front();

    if (!registry.valid(player_entity))
        return;

    auto& player_data     = registry.get<Player>(player_entity);
    player_data.game_over = true;
}
