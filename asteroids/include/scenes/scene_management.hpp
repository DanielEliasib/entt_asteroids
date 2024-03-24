#ifndef SCENE_MANAGEMENT
#define SCENE_MANAGEMENT

#include <entt/entt.hpp>
#include <memory>

#include "components/asteroid.hpp"
#include "components/enemy.hpp"
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

static const void create_score_scene(std::shared_ptr<state>& scene_state, std::shared_ptr<entt::registry> registry)
{
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

        int score = 0;

        {
            auto player_view   = registry->view<Player>();
            auto player_entity = player_view.front();
            if (registry->valid(player_entity))
            {
                auto& player_data = player_view.get<Player>(player_entity);
                score             = player_data.score + player_data.lives * 500;
            }
        }

        float screenWidth  = GetScreenWidth();
        float screenHeight = GetScreenHeight();

        const char* title_text = TextFormat("FINAL SCORE: %d", score);

        int fontSize  = 40;
        int textWidth = MeasureText(title_text, fontSize);

        Vector2 position = Vector2{(screenWidth - textWidth) / 2.0f,
                                   screenHeight / 2.0f - 100.0f};

        make_text(title_text, fontSize, position, RAYWHITE);
        make_text(title_text, fontSize, position + Vector2{5, 5}, BLACK);

        const char* subtitle_text = "Thanks For Playing!";

        fontSize  = 30;
        textWidth = MeasureText(subtitle_text, fontSize);

        position = Vector2{(screenWidth - textWidth) / 2.0f,
                           screenHeight / 2.0f - 50.0f};

        make_text(subtitle_text, fontSize, position, RAYWHITE);
        make_text(subtitle_text, fontSize, position + Vector2{5, 5}, BLACK);

        const char* exit_text = "Press ESC to close";

        fontSize  = 30;
        textWidth = MeasureText(exit_text, fontSize);

        position = Vector2{(screenWidth - textWidth) / 2.0f,
                           screenHeight / 2.0f + 10.0f};

        make_text(exit_text, fontSize, position, RED);
        make_text(exit_text, fontSize, position + Vector2{5, 5}, BLACK);

        const char* restart_text = "Press SPACE to restart";

        fontSize  = 20;
        textWidth = MeasureText(restart_text, fontSize);

        position = Vector2{(screenWidth - textWidth) / 2.0f,
                           screenHeight / 2.0f + 50.0f};

        make_text(restart_text, fontSize, position, text_color);
        make_text(restart_text, fontSize, position + Vector2{5, 5}, BLACK);
    };

    auto on_exit = [registry, general_scheduler, render_scheduler]() {
        auto texture_view = registry->view<Texture2D>();

        for (auto [entity, texture] : texture_view.each())
        {
            UnloadTexture(texture);
        }

        general_scheduler->clear();
        render_scheduler->clear();

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
inline const void create_game_scene(std::shared_ptr<state>& scene_state, std::shared_ptr<entt::registry>& registry)
{
    registry = std::make_shared<entt::registry>();

    std::shared_ptr<input_handler> input = std::make_shared<input_handler>(*registry);

    std::shared_ptr<entt::scheduler> general_scheduler = std::make_shared<entt::scheduler>();
    std::shared_ptr<entt::scheduler> render_scheduler  = std::make_shared<entt::scheduler>();
    std::shared_ptr<entt::scheduler> cleanup_scheduler = std::make_shared<entt::scheduler>();

    auto on_enter = [registry, input, general_scheduler, render_scheduler, cleanup_scheduler]() {
        general_scheduler->attach<lifetime_process>(*registry);
        general_scheduler->attach<enemy_ai_process>(*registry);
        general_scheduler->attach<trail_update_process>(*registry);
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
        LoadTextureToEntity<GAME_TEXTURES::SMOKETEXTURE>("asteroids/resources/smoke_fx.png", *registry);
        LoadTextureToEntity<GAME_TEXTURES::BULLETTEXTURE_BLUE>("asteroids/resources/bullet_blue.png", *registry);
        LoadTextureToEntity<GAME_TEXTURES::BULLETTEXTURE_RED>("asteroids/resources/bullet_red.png", *registry);

        // INFO: Create player
        spawn_main_camera(*registry);
        create_player(*registry, 0);

        // INFO: Create enemies
        spawn_random_asteroid_distribution(*registry, 4);
        spawn_random_enemy(*registry);

        spawn_game_ui(*registry);

        spawn_random_start_distribution(*registry, 30);
    };

    auto on_exit = [registry, input, general_scheduler, render_scheduler, cleanup_scheduler]() {
        auto texture_view = registry->view<Texture2D>();

        for (auto [entity, texture] : texture_view.each())
        {
            UnloadTexture(texture);
        }
        Player player_data;

        auto player_view   = registry->view<Player>();
        auto player_entity = player_view.front();

        if (registry->valid(player_entity))
        {
            player_data = player_view.get<Player>(player_entity);
        }

        general_scheduler->clear();
        render_scheduler->clear();
        cleanup_scheduler->clear();

        registry->clear();

        if (player_data.game_over)
            return;

        auto new_entity = registry->create();
        registry->emplace<Player>(new_entity, player_data);
    };

    auto on_update = [registry, input, general_scheduler, render_scheduler, cleanup_scheduler](float delta_time) {
        const uint32_t delta_time_ms = delta_time * 1000;

        auto trailer_view = registry->view<entt::tag<player_trail_tag>, sprite_render>();

        for (auto [entity, sprite] : trailer_view.each())
        {
            sprite.tint = Color{0, 0, 0, 0};
        }

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
    std::shared_ptr<state> score_scene;

    std::shared_ptr<entt::registry> game_registry;

    create_game_scene(game_scene, game_registry);
    create_title_scene(title_scene);
    create_score_scene(score_scene, game_registry);

    state_machine game_state_machine(title_scene);

    title_scene->add_transition([]() {
        return IsKeyPressed(KEY_SPACE);
    },
                                game_scene, "TITLE TO GAME");

    game_scene->add_transition([game_registry]() {
        auto player_view   = game_registry->view<Player>();
        auto player_entity = player_view.front();

        if (!game_registry->valid(player_entity))
            return false;

        auto& player_data = game_registry->get<Player>(player_entity);

        return player_data.game_over && IsKeyPressed(KEY_SPACE);
    },
                               game_scene, "GAME TO GAME");

    game_scene->add_transition([game_registry]() { 
			auto asteroid_view = game_registry->view<asteroid>();
			auto enemy_view = game_registry->view<entt::tag<enemy_tag>>();
			int count = asteroid_view.size() + enemy_view.size();
			return count <= 0; },
                               score_scene, "GAME TO SCORE");

    score_scene->add_transition([game_registry]() {
        return IsKeyPressed(KEY_SPACE);
    },
                                game_scene, "SCORE TO GAME");

    return game_state_machine;
}

#endif // SCENE_MANAGEMENT
