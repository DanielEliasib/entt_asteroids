#ifndef RENDER_PROCESSORS_HPP
#define RENDER_PROCESSORS_HPP

#include <raylib.h>
#include <rlgl.h>

#include <components/base.hpp>
#include <components/render.hpp>
#include <entt/entt.hpp>
#include <iostream>
#include <variant>

#include "player.hpp"

struct ui_process : entt::process<ui_process, std::uint32_t>
{
    using delta_type = std::uint32_t;

    ui_process(entt::registry& registry) :
        registry(registry) {}

    void update(delta_type delta_time, void*)
    {
        auto player_entity = registry.view<Player>().front();

        if (!registry.valid(player_entity))
        {
            return;
        }

        auto player_data = registry.get<Player>(player_entity);

        DrawText(TextFormat("Score: %i", player_data.score), 10, 70, 20, WHITE);
    }

   protected:
    entt::registry& registry;
};

struct render_process : entt::process<render_process, std::uint32_t>
{
    using delta_type = std::uint32_t;

    render_process(entt::registry& registry) :
        registry(registry) {}

    void update(delta_type delta_time, void*)
    {
        auto render_view = registry.view<transform, shape_render>();

        for (auto [entity, transform_data, render_data] : render_view.each())
        {
            rlPushMatrix();
            rlTranslatef(transform_data.position.x, transform_data.position.y, 0.0f);
            rlRotatef(transform_data.rotation, 0.0f, 0.0f, 1.0f);

            switch (render_data.shape)
            {
                case render_shape_type::TRIANGLE: {
                    auto data = static_cast<Vector2*>(render_data.data);
                    std::vector<Vector2> vertices(data, data + 3);

                    DrawTriangle(vertices[0], vertices[1], vertices[2],
                                 render_data.color);

                    DrawCircleV(vertices[0], 1.4f, GREEN);
                    DrawCircleV(vertices[1], 1.4f, RED);
                    DrawCircleV(vertices[2], 1.6f, BLUE);

                    DrawTriangleLines(vertices[0], vertices[1], vertices[2], BLACK);
                    break;
                }
                case render_shape_type::CIRCLE: {
                    auto radius = *static_cast<float*>(render_data.data);
                    DrawCircleV(Vector2{0, 0}, radius, render_data.color);
                    break;
                }
            }

            rlPopMatrix();
        }
    }

   protected:
    entt::registry& registry;
};

struct camera_process : entt::process<camera_process, std::uint32_t>
{
    using delta_type = std::uint32_t;

    camera_process(entt::registry& registry) :
        registry(registry) {}

    void update(delta_type delta_time, void*)
    {
        auto camera_entity = registry.view<Camera2D>().front();
        auto player_entity = registry.view<Player, transform>().front();

        auto& camera_data           = registry.get<Camera2D>(camera_entity);
        auto& player_transform_data = registry.get<transform>(player_entity);

        // camera_data.target = player_transform_data.position;
    }

   protected:
    entt::registry& registry;
};

#endif // RENDER_PROCESSORS_HPP
