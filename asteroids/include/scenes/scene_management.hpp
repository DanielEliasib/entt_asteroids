#ifndef SCENE_MANAGEMENT
#define SCENE_MANAGEMENT

#include <entt/entt.hpp>
#include <memory>

#include "components/asteroid.hpp"
#include "components/player.hpp"
#include "processors/base_processors.hpp"
#include "processors/enemy_processors.hpp"
#include "processors/physics_processors.hpp"
#include "processors/render_processors.hpp"
#include "raylib.h"
#include "raymath.h"
#include "utils/input_handler.hpp"
#include "utils/state.hpp"

static const Color background_color = {15, 15, 15, 255};
static const Color text_color       = {204, 191, 147, 255};

static const void create_title_scene(std::shared_ptr<state>& scene_state)
{
    std::shared_ptr<entt::registry> registry = std::make_shared<entt::registry>();

    std::shared_ptr<entt::scheduler> general_scheduler = std::make_shared<entt::scheduler>();
    std::shared_ptr<entt::scheduler> render_scheduler  = std::make_shared<entt::scheduler>();

    auto on_enter = [registry, general_scheduler, render_scheduler]() {
        general_scheduler->attach<physics_process>(*registry);
        general_scheduler->attach<boundary_process>(*registry);

        render_scheduler->attach<text_render_process>(*registry);
        render_scheduler->attach<sprite_render_process>(*registry);
        render_scheduler->attach<sprite_sequence_process>(*registry);

        // INFO: Load textures
        LoadTextureToEntity<GAME_TEXTURES::MAINTEXTURE>("asteroids/resources/simpleSpace_tilesheet.png", *registry);

        // INFO: Create player
        spawn_main_camera(*registry);

        // INFO: Create enemies
        spawn_random_asteroid_distribution(*registry, 4);

        // INFO: Create title text
        auto make_text = [registry](const char* text, int font_size, Vector2 position, Color color) {
            auto text_entity = registry->create();

            text_render text_component;
            text_component.font      = GetFontDefault();
            text_component.text      = text;
            text_component.font_size = font_size;
            text_component.color     = color;

            int textWidth = MeasureText(text_component.text, text_component.font_size);

            transform text_transform;
            text_transform.position = position;
            text_transform.rotation = 0.0f;

            registry->emplace<transform>(text_entity, text_transform);
            registry->emplace<text_render>(text_entity, text_component);
        };

        float screenWidth  = GetScreenWidth();
        float screenHeight = GetScreenHeight();

        const char* title_text = "ASTEROIDS";

        int fontSize  = 40;
        int textWidth = MeasureText(title_text, fontSize);

        Vector2 position = Vector2{(screenWidth - textWidth) / 2.0f,
                                   screenHeight / 2.0f - 100.0f};

        make_text(title_text, fontSize, position, RAYWHITE);
        make_text(title_text, fontSize, position + Vector2{5, 5}, BLACK);

        const char* subtitle_text = "Press SPACE to start";

        fontSize  = 30;
        textWidth = MeasureText(subtitle_text, fontSize);

        position = Vector2{(screenWidth - textWidth) / 2.0f,
                           screenHeight / 2.0f - 50.0f};

        make_text(subtitle_text, fontSize, position, text_color);
        make_text(subtitle_text, fontSize, position + Vector2{5, 5}, BLACK);
    };

    auto on_exit = [registry]() {
        auto texture_view = registry->view<Texture2D>();

        for (auto [entity, texture] : texture_view.each())
        {
            UnloadTexture(texture);
        }

        registry->clear();
    };

    auto on_update = [registry, general_scheduler, render_scheduler](float delta_time) {
        const uint32_t delta_time_ms = delta_time * 1000;

        general_scheduler->update(delta_time_ms);

        BeginDrawing();
        ClearBackground(background_color);

        Camera2D camera = registry->get<Camera2D>(registry->view<Camera2D>().front());
        BeginMode2D(camera);

        render_scheduler->update(delta_time_ms);

        EndMode2D();
        EndDrawing();
    };

    scene_state = std::make_shared<state>(on_enter, on_exit, on_update);
}

inline const void create_game_scene(std::shared_ptr<state>& scene_state)
{
    std::shared_ptr<entt::registry> registry = std::make_shared<entt::registry>();

    std::shared_ptr<input_handler> input = std::make_shared<input_handler>(*registry);

    std::shared_ptr<entt::scheduler> general_scheduler = std::make_shared<entt::scheduler>();
    std::shared_ptr<entt::scheduler> render_scheduler  = std::make_shared<entt::scheduler>();
    std::shared_ptr<entt::scheduler> cleanup_scheduler = std::make_shared<entt::scheduler>();

    auto on_enter = [registry, input, general_scheduler, render_scheduler, cleanup_scheduler]() {
        general_scheduler->attach<lifetime_process>(*registry);
        general_scheduler->attach<enemy_ai_process>(*registry);
        general_scheduler->attach<physics_process>(*registry);
        general_scheduler->attach<collision_process>(*registry);
        general_scheduler->attach<boundary_process>(*registry);

        render_scheduler->attach<text_render_process>(*registry);
        render_scheduler->attach<sprite_render_process>(*registry);
        render_scheduler->attach<sprite_sequence_process>(*registry);
        render_scheduler->attach<shape_render_process>(*registry);

        cleanup_scheduler->attach<cleanup_process>(*registry);

        // INFO: Load textures
        LoadTextureToEntity<GAME_TEXTURES::MAINTEXTURE>("asteroids/resources/simpleSpace_tilesheet.png", *registry);
        LoadTextureToEntity<GAME_TEXTURES::PLANETEXTURE>("asteroids/resources/simplePlanes_tilesheet.png", *registry);

        // INFO: Create player
        spawn_main_camera(*registry);
        create_player(*registry, 0);

        // INFO: Create enemies
        spawn_random_asteroid_distribution(*registry, 4);
        spawn_random_enemy(*registry);

        spawn_game_ui(*registry);
    };

    auto on_exit = [registry]() {
        auto texture_view = registry->view<Texture2D>();

        for (auto [entity, texture] : texture_view.each())
        {
            UnloadTexture(texture);
        }

        registry->clear();
    };

    auto on_update = [registry, input, general_scheduler, render_scheduler, cleanup_scheduler](float delta_time) {
        const uint32_t delta_time_ms = delta_time * 1000;

        input->handle_input();

        general_scheduler->update(delta_time_ms);

        BeginDrawing();
        ClearBackground(background_color);

        Camera2D camera = registry->get<Camera2D>(registry->view<Camera2D>().front());
        BeginMode2D(camera);

        render_scheduler->update(delta_time_ms);

        EndMode2D();
        EndDrawing();

        cleanup_scheduler->update(delta_time_ms);
    };

    scene_state = std::make_shared<state>(on_enter, on_exit, on_update);
}

static const state_machine create_game_state_machine()
{
    std::shared_ptr<state> title_scene;
    std::shared_ptr<state> game_scene;
    create_game_scene(game_scene);
    create_title_scene(title_scene);

    state_machine game_state_machine(title_scene);

    title_scene->add_transition([]() {
        return IsKeyPressed(KEY_SPACE);
    },
                                game_scene);
    // game_state_machine.add_transition(game_scene, title_scene, "exit", []() { return true; });

    return game_state_machine;
}

#endif // SCENE_MANAGEMENT
