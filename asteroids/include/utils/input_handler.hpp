#ifndef INPUT_HANDLER_HPP
#define INPUT_HANDLER_HPP

#include <entt/entt.hpp>

#include "utils/events.hpp"

class input_handler {
   public:
    input_handler(entt::registry& registry);
    ~input_handler();

    void handle_input();

   private:
    event* acceleration_button_pressed;
    entt::registry& registry;
};

#endif // INPUT_HANDLER_HPP
