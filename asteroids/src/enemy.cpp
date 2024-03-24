#include "components/enemy.hpp"

#include <components/base.hpp>
#include <components/player.hpp>
#include <components/render.hpp>
#include <memory>

#include "components/physics.hpp"
#include "math.hpp"
#include "utils/state.hpp"

void on_enemy_collision(entt::registry& registry, entt::entity bullet_entity, entt::entity enemy_entity)
{
    auto enemy_physics   = registry.get<physics>(enemy_entity);
    auto enemy_transform = registry.get<transform>(enemy_entity);

    if (registry.valid(enemy_entity))
    {
        registry.emplace<entt::tag<kill_tag>>(enemy_entity);
        spawn_explosion(registry, enemy_transform.position, 3);
    }
}

void spawn_random_enemy(entt::registry& registry)
{
    const float screenWidth  = GetScreenWidth();
    const float screenHeight = GetScreenHeight();

    const Rectangle top_rect    = {-40, -40, screenWidth + 40.0f, 40};
    const Rectangle bottom_rect = {-40, screenHeight, screenWidth + 40.0f, 40};
    const Rectangle left_rect   = {-40, -40, 40, screenHeight + 40.0f};
    const Rectangle right_rect  = {screenWidth, -40, 40, screenHeight + 40.0f};

    const std::array<Rectangle, 4> rects = {top_rect, bottom_rect, left_rect, right_rect};

    int random            = GetRandomValue(0, 3);
    const Rectangle& rect = rects[random];

    float x = GetRandomValue(rect.x, rect.x + rect.width);
    float y = GetRandomValue(rect.y, rect.y + rect.height);

    spawn_enemy(registry, {x, y});
}

entt::entity spawn_enemy(entt::registry& registry, Vector2 position)
{
    static const std::uint32_t texture_tag = static_cast<std::uint32_t>(GAME_TEXTURES::MAINTEXTURE);
    entt::entity entity                    = registry.create();

    registry.emplace<transform>(
        entity,
        transform{
            // Vector2{100, 100},
            position,
            Vector2{0, 1}, 0});
    registry.emplace<physics>(entity, physics{Vector2{0, 0}, 0, 0.005f, Vector2{0, 0}, Vector2{0, 0}});
    registry.emplace<entt::tag<enemy_tag>>(entity);
    registry.emplace<team>(entity, team::ENEMY);

    circle_collider enemy_collider;
    enemy_collider.radius = 10;

    registry.emplace<circle_collider>(entity, enemy_collider);

    bullet_collision_response player_collision_responder;
    player_collision_responder.on_collision.connect<&on_enemy_collision>();

    registry.emplace<bullet_collision_response>(entity, player_collision_responder);

    auto texture_entity = registry.view<Texture2D, entt::tag<texture_tag>>().front();
    Texture2D tilesheet = registry.get<Texture2D>(texture_entity);

    float scale = 10 * 2 / 96.0f;
    registry.emplace<sprite_render>(entity, sprite_render{tilesheet, Rectangle{912, 144, 96, 96}, scale});

    // INFO: AI DEFINITION -----------------------------------------------------
    // INFO: CHASING STATE
    // NOTE: How could we pass the specific entity to the state update function?
    static auto on_chasing_update = [&registry](float delta_time) {
        auto player_view = registry.view<entt::tag<player_tag>, transform>();
        auto enemy_view  = registry.view<entt::tag<enemy_tag>, transform, physics>();

        auto player_entity = player_view.front();
        auto entity        = enemy_view.front();

        if (!registry.valid(player_entity) || !registry.valid(entity))
        {
            return;
        }

        auto& player_transform = player_view.get<transform>(player_entity);

        auto& enemy_transform = registry.get<transform>(entity);
        auto& enemy_physics   = registry.get<physics>(entity);

        Vector2 direction = Vector2Subtract(player_transform.position, enemy_transform.position);
        direction         = Vector2Normalize(direction);

        enemy_physics.external_impulse = enemy_physics.external_impulse + direction * 120.0f;
    };

    std::shared_ptr<state> chasing_state = std::make_shared<state>(nullptr, nullptr, on_chasing_update);

    // INFO: ATTACK STATE
    std::shared_ptr<float> attack_elapsed_time = std::make_shared<float>(0);
    static auto on_attack_update               = [&registry, attack_elapsed_time](float delta_time) {
        float temp           = *attack_elapsed_time + delta_time;
        *attack_elapsed_time = temp;

        if (*attack_elapsed_time > 0.65f)
        {
            *attack_elapsed_time = 0;

            auto player_view = registry.view<entt::tag<player_tag>, transform>();
            auto enemy_view  = registry.view<entt::tag<enemy_tag>, transform, physics>();

            auto player_entity = player_view.front();
            auto entity        = enemy_view.front();

            if (!registry.valid(player_entity) || !registry.valid(entity))
            {
                return;
            }

            auto& player_transform = player_view.get<transform>(player_entity);
            auto& enemy_transform  = registry.get<transform>(entity);
            auto& enemy_physics    = registry.get<physics>(entity);

            Vector2 direction = Vector2Subtract(player_transform.position, enemy_transform.position);
            direction         = Vector2Normalize(direction);

            auto bullet_velocity = enemy_physics.velocity + direction * 320.0f;

            spawn_bullet(registry, enemy_transform.position, bullet_velocity, team::ENEMY);
        }
    };

    std::shared_ptr<state> attack_state = std::make_shared<state>(nullptr, nullptr, on_attack_update);

    // INFO: FROM CHASING TO ATTACK
    static auto attack_condition = [&registry]() {
        auto player_view = registry.view<entt::tag<player_tag>, transform>();
        auto enemy_view  = registry.view<entt::tag<enemy_tag>, transform, physics>();

        auto player_entity = player_view.front();
        auto entity        = enemy_view.front();

        if (!registry.valid(player_entity) || !registry.valid(entity))
        {
            return false;
        }

        auto& player_transform = player_view.get<transform>(player_entity);
        auto& enemy_transform  = registry.get<transform>(entity);

        static const float radius = 200;
        float sqr_distance        = Vector2DistanceSqr(player_transform.position, enemy_transform.position);

        return sqr_distance < radius * radius;
    };
    chasing_state->add_transition(attack_condition, attack_state);

    // INFO: FROM ATTACK TO CHASING
    static auto chasing_condition = [&registry]() {
        return !attack_condition();
    };
    attack_state->add_transition(chasing_condition, chasing_state);

    auto ai_machine = std::make_shared<state_machine>(chasing_state);
    registry.emplace<enemy_ai>(entity, ai_machine);
    // INFO: END AI DEFINITION -------------------------------------------------

    return entity;
}
