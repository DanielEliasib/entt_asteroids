#ifndef RENDER_PROCESSORS_HPP
#define RENDER_PROCESSORS_HPP

#include <raylib.h>
#include <rlgl.h>

#include <components/base.hpp>
#include <components/render.hpp>
#include <entt/entt.hpp>
#include <iostream>

struct render_process : entt::process<render_process, std::uint32_t>
{
    using delta_type = std::uint32_t;

    render_process(entt::registry& registry) :
        registry(registry) {}

    void update(delta_type delta_time, void*)
    {
        auto render_view = registry.view<transform, render>();

        for (auto [entity, transform_data, render_data] : render_view.each())
        {
            std::vector<Vector2> vertices = render_data.points;

            rlPushMatrix();
            rlTranslatef(transform_data.position.x, transform_data.position.y, 0.0f);
            rlRotatef(transform_data.rotation, 0.0f, 0.0f, 1.0f);

            // TODO: Add support for non triangles
            if (vertices.size() == 3)
            {
                DrawTriangle(vertices[0], vertices[1], vertices[2],
                             render_data.color);

                DrawCircleV(vertices[0], 1.4f, GREEN);
                DrawCircleV(vertices[1], 1.4f, RED);
                DrawCircleV(vertices[2], 1.6f, BLUE);

                DrawTriangleLines(vertices[0], vertices[1], vertices[2], BLACK);
            } else
            {
                DrawCircleV(vertices[0], 10, RED);
            }

            rlPopMatrix();
        }
    }

   protected:
    entt::registry& registry;
};

#endif // RENDER_PROCESSORS_HPP
