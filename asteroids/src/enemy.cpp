#include "components/enemy.hpp"

#include <components/base.hpp>
#include <components/player.hpp>
#include <components/render.hpp>
#include <iostream>
#include <memory>

#include "math.hpp"
#include "utils/state.hpp"

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

    auto texture_entity = registry.view<Texture2D, entt::tag<texture_tag>>().front();
    Texture2D tilesheet = registry.get<Texture2D>(texture_entity);

    float scale = 10 * 2 / 96.0f;
    registry.emplace<sprite_render>(entity, sprite_render{tilesheet, Rectangle{912, 144, 96, 96}, scale});

    // INFO: AI DEFINITION -----------------------------------------------------
    // INFO: CHASING STATE
    // NOTE: How could we pass the specific entity to the state update function?
    static auto on_chasing_update = [&registry]() {
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

        std::cout << "CHASING" << std::endl;
    };

    std::shared_ptr<state> chasing_state = std::make_shared<state>(nullptr, nullptr, on_chasing_update);

    // INFO: ATTACK STATE
    static auto on_attack_update = []() {
        std::cout << "ATTACKING" << std::endl;
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
    static auto chasing_condition = [&registry]() { return !attack_condition(); };
    attack_state->add_transition(chasing_condition, chasing_state);

    state_machine ai_machine = state_machine(chasing_state);

    registry.emplace<enemy_ai>(entity, ai_machine);
    // INFO: END AI DEFINITION -------------------------------------------------

    return entity;
}
