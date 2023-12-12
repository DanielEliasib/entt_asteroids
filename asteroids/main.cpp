#include <raylib.h>

#include <iostream>

int main()
{
    const char* TITLE = "ASTEROIDS";
    InitWindow(800, 600, TITLE);

    Color background_color = {15, 15, 15, 255};
    Color text_color       = {204, 191, 147, 255};

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(background_color);
        DrawText(TITLE, 10, 10, 20, text_color);
        EndDrawing();
    }

    return 0;
}
