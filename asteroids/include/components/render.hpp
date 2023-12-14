#ifndef RENDER_HPP
#define RENDER_HPP

#include <entt/entt.hpp>

#include "raylib.h"

struct render
{
    Color color;
    std::vector<Vector2> points;
};

inline static void add_render_data(entt::registry& registry, entt::entity entity, Color color)
{
    int side     = 10;
    float height = sqrt(pow(side, 2) - pow(side / 2, 2));

    Vector2 v1 = Vector2{0 - height / 2, 0 - side / 2.0f};
    Vector2 v2 = Vector2{0 - height / 2, 0 + side / 2.0f};
    Vector2 v3 = Vector2{0 + height / 2, 0};

    std::vector<Vector2> original_triangle = {v1, v2, v3};

    render render_data{color, original_triangle};
    registry.emplace<render>(entity, render_data);
}

#endif // RENDER_HPP
