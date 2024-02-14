#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <raylib.h>

#include <chrono>
#include <entt/entt.hpp>

#include "components/base.hpp"
#include "components/player.hpp"


struct Vector2;

class command {
   public:
    command(entt::registry& registry) :
        registry(registry) {}
    virtual ~command() {}
    virtual void execute(Vector2 input) = 0;

   protected:
    entt::registry& registry;
};

class acceleration_input_command : public command {
   public:
    acceleration_input_command(entt::registry& registry) :
        command(registry) {}
    void execute(Vector2 input) override;
};

class mouse_input_command : public command {
   public:
    mouse_input_command(entt::registry& registry) :
        command(registry) {}
    void execute(Vector2 input) override;
};

class shoot_input_command : public command {
   public:
    shoot_input_command(entt::registry& registry) :
        command(registry)
    {
        auto player_entity = registry.view<Player, transform>().front();
        if (registry.valid(player_entity))
        {
            _player_entity = player_entity;
        }

        _cooldown = _short_cooldown;
    }
    void execute(Vector2 input) override;

   protected:
    entt::entity _player_entity = entt::null;
    std::chrono::high_resolution_clock::time_point _last_shot_time;

    uint16_t _shots_fired = 0;
    uint16_t _max_shots   = 3;

    uint16_t _cooldown = 0;

    uint16_t _short_cooldown = 150;
    uint16_t _long_cooldown  = 800;
};

#endif // COMMAND_HPP
