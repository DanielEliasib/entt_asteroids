#ifndef RENDER_PROCESSORS_HPP
#define RENDER_PROCESSORS_HPP

#include <raylib.h>
#include <rlgl.h>

#include <components/base.hpp>
#include <components/render.hpp>
#include <entt/entt.hpp>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "components/player.hpp"

struct text_render_process : entt::process<text_render_process, std::uint32_t>
{
    using delta_type = std::uint32_t;

    text_render_process(entt::registry& registry) :
        registry(registry) {}

    void update(delta_type delta_time, void*)
    {
        auto text_view = registry.view<transform, text_render>();

        for (auto [entity, transform_data, render_data] : text_view.each())
        {
            rlPushMatrix();
            rlTranslatef(transform_data.position.x, transform_data.position.y, 0.0f);
            rlRotatef(transform_data.rotation, 0.0f, 0.0f, 1.0f);

            DrawText(render_data.text,
                     0, 0, render_data.font_size, render_data.color);

            rlPopMatrix();
        }

        auto dynamic_text_view = registry.view<transform, dynamic_text_render>();

        for (auto [entity, transform_data, render_data] : dynamic_text_view.each())
        {
            rlPushMatrix();
            rlTranslatef(transform_data.position.x, transform_data.position.y, 0.0f);
            rlRotatef(transform_data.rotation, 0.0f, 0.0f, 1.0f);

            auto text = render_data.text_function(registry);

            Vector2 position = {0, 0};

            if (render_data.center)
            {
                int width  = MeasureText(text, render_data.font_size);
                position.x = -width / 2;
            }

            DrawText(text, position.x, position.y, render_data.font_size, render_data.color);

            rlPopMatrix();
        }
    }

   protected:
    entt::registry& registry;
};

struct shape_render_process : entt::process<shape_render_process, std::uint32_t>
{
    using delta_type = std::uint32_t;

    shape_render_process(entt::registry& registry) :
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

struct sprite_render_process : entt::process<sprite_render_process, std::uint32_t>
{
    using delta_type = std::uint32_t;

    sprite_render_process(entt::registry& registry) :
        registry(registry) {}

    void update(delta_type delta_time, void*)
    {
        auto render_view = registry.view<transform, sprite_render>();

        for (auto [entity, transform_data, render_data] : render_view.each())
        {
            if (!IsTextureReady(render_data.texture))
            {
                continue;
            }

            rlPushMatrix();
            rlTranslatef(transform_data.position.x, transform_data.position.y, 0.0f);
            rlRotatef(transform_data.rotation, 0.0f, 0.0f, 1.0f);

            DrawTexturePro(
                render_data.texture,
                render_data.source,
                Rectangle{
                    0,
                    0,
                    render_data.source.width * render_data.scale, render_data.source.height * render_data.scale},
                Vector2{render_data.source.width * render_data.scale / 2, render_data.source.height * render_data.scale / 2} + render_data.offset,
                90, render_data.tint);

            rlPopMatrix();
        }
    }

   protected:
    entt::registry& registry;
};

struct sprite_sequence_process : entt::process<sprite_sequence_process, std::uint32_t>
{
    using delta_type = std::uint32_t;

    sprite_sequence_process(entt::registry& registry) :
        registry(registry) {}

    void update(delta_type delta_time, void*)
    {
        auto render_view = registry.view<sprite_sequence, sprite_render>();

        float delta_time_seconds = delta_time / 1000.0f;

        for (auto [entity, sequence_data, render_data] : render_view.each())
        {
            if (!sequence_data.update)
                continue;

            sequence_data.current_delta += delta_time_seconds;

            if (sequence_data.current_delta >= sequence_data.frame_time)
            {
                sequence_data.current_delta = 0;
                sequence_data.current_frame = (sequence_data.current_frame + 1) % sequence_data.frame_count;

                if (sequence_data.current_frame == 0)
                {
                    if (sequence_data.loop)
                    {
                        sequence_data.frames = sequence_data.first_frame;
                    } else
                    {
                        sequence_data.update = false;
                    }
                } else
                {
                    sequence_data.frames++;
                }

                render_data.source = sequence_data.frames->source;
            }
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
        auto player_entity = registry.view<entt::tag<player_tag>, transform>().front();

        auto& camera_data           = registry.get<Camera2D>(camera_entity);
        auto& player_transform_data = registry.get<transform>(player_entity);

        // camera_data.target = player_transform_data.position;
    }

   protected:
    entt::registry& registry;
};

#endif // RENDER_PROCESSORS_HPP
