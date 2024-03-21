#ifndef ENEMY_PROCESSORS_HPP
#define ENEMY_PROCESSORS_HPP

#include <raylib.h>

#include <components/enemy.hpp>
#include <entt/entt.hpp>


struct enemy_ai_process : entt::process<enemy_ai_process, std::uint32_t>
{
    using delta_type = std::uint32_t;

    enemy_ai_process(entt::registry& registry) :
        registry(registry) {}

    void update(delta_type delta_time, void*)
    {
        auto view = registry.view<enemy_ai>();
        for (auto entity : view)
        {
            auto& ai = view.get<enemy_ai>(entity);
            ai.ai_machine.update();
        }
    }

    entt::registry& registry;
};

#endif // ENEMY_PROCESSORS_HPP
