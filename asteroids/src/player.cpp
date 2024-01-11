#include <player.hpp>

#include "components/asteroid.hpp"
#include "components/render.hpp"

entt::entity create_player(entt::registry& registry, uint8_t id)
{
    entt::entity entity = registry.create();
    registry.emplace<Player>(entity, Player{id});
    registry.emplace<transform>(entity, transform{Vector2{400, 300}, Vector2{0, 1}, 0});
    registry.emplace<physics>(entity, physics{Vector2{0, 0}, 0, 0.005f, Vector2{0, 0}, Vector2{0, 0}});

    add_render_data(registry, entity, WHITE);

    return entity;
}

static std::unique_ptr<float> radius_ptr = std::make_unique<float>(1.5f);

entt::entity spawn_bullet(entt::registry& registry, Vector2 position, Vector2 velocity)
{
    entt::entity entity = registry.create();
    float angle         = atan2(velocity.y, velocity.x) * RAD2DEG;

    shape_render render_data;
    render_data.color = RED;
    render_data.shape = render_shape_type::CIRCLE;
    render_data.data  = static_cast<void*>(radius_ptr.get());

    registry.emplace<transform>(entity, transform{position, Vector2{0, 1}, angle});
    registry.emplace<physics>(entity, physics{velocity, 0, 0.0f, Vector2{0, 0}, Vector2{0, 0}});
    registry.emplace<shape_render>(entity, render_data);

    return entity;
}

static std::unique_ptr<float> a_radius_2_ptr = std::make_unique<float>(50.0f);
static std::unique_ptr<float> a_radius_1_ptr = std::make_unique<float>(25.0f);
static std::unique_ptr<float> a_radius_0_ptr = std::make_unique<float>(10.0f);
entt::entity spawn_asteroid(entt::registry& registry, Vector2 position, Vector2 velocity, int8_t level)
{
    auto level_radius = [](int8_t level) {
        if (level >= 2)
        {
            return a_radius_2_ptr.get();
        } else if (level >= 1)
        {
            return a_radius_1_ptr.get();
        } else
        {
            return a_radius_0_ptr.get();
        }
    };

    if (level < 0)
    {
        return entt::null;
    }

    entt::entity entity = registry.create();
    float angle         = atan2(velocity.y, velocity.x) * RAD2DEG;
    auto radius         = level_radius(3);

    shape_render render_data;
    render_data.color = WHITE;
    render_data.shape = render_shape_type::CIRCLE;
    render_data.data  = static_cast<void*>(radius);

    registry.emplace<transform>(entity, transform{position, Vector2{0, 1}, angle});
    registry.emplace<physics>(entity, physics{velocity, 0, 0.0f, Vector2{0, 0}, Vector2{0, 0}});
    registry.emplace<shape_render>(entity, render_data);
    registry.emplace<asteroid>(entity, asteroid{level});

    return entity;
}
