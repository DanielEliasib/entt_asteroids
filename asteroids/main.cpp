#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <iostream>
#include <math.hpp>
#include <player.hpp>

int main()
{
    const char* TITLE = "ASTEROIDS";
    InitWindow(800, 600, TITLE);

    Color background_color = {15, 15, 15, 255};
    Color text_color       = {204, 191, 147, 255};

    entt::registry registry;
    create_player(registry, 0);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(background_color);
        DrawText(TITLE, 10, 10, 20, text_color);

        auto view = registry.view<Player, transform, render>();
        for (auto entity : view)
        {
            auto& player_data    = view.get<Player>(entity);
            auto& transform_data = view.get<transform>(entity);
            auto& render_data    = view.get<render>(entity);

            // TODO: Only support triangles for now
            auto vertices = render_data.points;

            rlPushMatrix();
            float angle =
                atan2(transform_data.direction.y, transform_data.direction.x) * RAD2DEG;
            rlTranslatef(transform_data.position.x, transform_data.position.y, 0.0f);
            rlRotatef(angle, 0.0f, 0.0f, 1.0f);

            DrawTriangle(vertices[0], vertices[1], vertices[2],
                         render_data.color);
            DrawCircleV(vertices[0], 1.4f, GREEN);
            DrawCircleV(vertices[1], 1.4f, RED);
            DrawCircleV(vertices[2], 1.6f, BLUE);
            DrawCircleV(Vector2Zero(), 1.2f, LIGHTGRAY);
            rlPopMatrix();
        }

        EndDrawing();
    }

    return 0;
}
