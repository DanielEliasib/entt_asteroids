#ifndef EVENT_HPP
#define EVENT_HPP

#include <entt/entt.hpp>

class event {
   public:
    event(entt::registry& registry) :
        registry(registry) {}
    virtual ~event() {}
    virtual void execute() = 0;

   protected:
    entt::registry& registry;
};

class acceleration_button_pressed_event : public event {
   public:
    acceleration_button_pressed_event(entt::registry& registry) :
        event(registry) {}
    void execute() override;
};

#endif // EVENT_HPP
