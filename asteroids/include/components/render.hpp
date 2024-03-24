#ifndef RENDER_HPP
#define RENDER_HPP

#include <cstdint>
#include <entt/entt.hpp>

#include "base.hpp"
#include "raylib.h"

enum class render_shape_type
{
    TRIANGLE,
    CIRCLE
};

struct shape_render
{
    Color color;
    render_shape_type shape;
    void* data;
};

struct sprite_render
{
    Texture2D texture;
    Rectangle source;
    float scale = 1.0f;
    Color tint  = WHITE;
};

struct sprite_frame
{
    Rectangle source;
};

struct sprite_sequence
{
    sprite_frame* frames;
    sprite_frame* first_frame;

    bool loop;
    bool update;

    int frame_count;
    int current_frame;

    float frame_time;
    float current_delta;
};

struct dynamic_text_render
{
    Font font;
    float font_size;
    Color color;
    bool center;

    std::function<const char*(entt::registry&)> text_function;
};

struct text_render
{
    Font font;
    const char* text;
    float font_size;
    Color color;
};

static entt::entity LoadFontToEntity(const char* path, entt::registry& registry)
{
    Font font = LoadFont(path);

    entt::entity entity = registry.create();
    registry.emplace<Font>(entity, font);

    return entity;
}

template<const GAME_TEXTURES val>
static entt::entity LoadTextureToEntity(const char* path, entt::registry& registry)
{
    Texture2D texture = LoadTexture(path);

    entt::entity entity = registry.create();

    registry.emplace<Texture2D>(entity, texture);
    registry.emplace<entt::tag<static_cast<std::uint32_t>(val)>>(entity);

    return entity;
}

static std::vector<Vector2>* original_triangle = nullptr;
inline static void add_render_data(entt::registry& registry, entt::entity entity, Color color)
{
    if (original_triangle == nullptr)
    {
        int side     = 10;
        float height = sqrt(pow(side, 2) - pow(side / 2, 2));

        Vector2 v1 = Vector2{0 - height / 2, 0 - side / 2.0f};
        Vector2 v2 = Vector2{0 - height / 2, 0 + side / 2.0f};
        Vector2 v3 = Vector2{0 + height / 2, 0};

        original_triangle = new std::vector<Vector2>{v1, v2, v3};
    }

    shape_render render_data;
    render_data.color = color;
    render_data.shape = render_shape_type::TRIANGLE;
    render_data.data  = static_cast<void*>(original_triangle->data());

    registry.emplace<shape_render>(entity, render_data);
}

inline static void spawn_main_camera(entt::registry& registry)
{
    entt::entity entity = registry.create();

    int screenWidth  = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    Camera2D camera;
    camera.target   = Vector2{screenWidth * 0.5f, screenHeight * 0.5f};
    camera.offset   = {static_cast<float>(GetScreenWidth() / 2), static_cast<float>(GetScreenHeight() / 2)};
    camera.rotation = 0.0f;
    camera.zoom     = 1.0f;

    registry.emplace<Camera2D>(entity, camera);
}

#endif // RENDER_HPP
