#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <entt/entt.hpp>

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

#endif // COMMAND_HPP
