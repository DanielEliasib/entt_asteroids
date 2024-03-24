#ifndef STATE_HPP
#define STATE_HPP

#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <ostream>

class state;

class state_transition {
   public:
    state_transition(std::function<bool()> condition, std::shared_ptr<state> target_state, const char* name) :
        _condition(condition),
        _target_state(target_state),
        name(name)
    {
        assert(_condition != nullptr);
        assert(_target_state != nullptr);
    };

    bool evaluate() const { return _condition(); };
    const char* get_name() const { return name; };
    std::shared_ptr<state> target_state() const { return _target_state; };

   protected:
    std::function<bool()> _condition     = nullptr;
    std::shared_ptr<state> _target_state = nullptr;
    const char* name                     = "transition";
};

class state {
   public:
    state(std::function<void()> on_enter, std::function<void()> on_exit, std::function<void(float)> update) :
        _on_enter(on_enter),
        _on_exit(on_exit),
        _update(update){};

    void add_transition(std::function<bool()> condition, std::shared_ptr<state> target_state, const char* name)
    {
        _transitions.push_back(state_transition(condition, target_state, name));
    }

   protected:
    void update(float delta_time)
    {
        if (_update != nullptr)
        {
            _update(delta_time);
        }
    };

    bool evaluate_transitions(std::shared_ptr<state>& next_state)
    {
        for (auto& transition : _transitions)
        {
            if (!transition.evaluate())
                continue;

            std::cout << "Transitioning to " << transition.get_name() << std::endl;

            on_exit();
            next_state = transition.target_state();
            next_state->on_enter();
            return true;
        }

        return false;
    }

    void on_enter()
    {
        if (_on_enter != nullptr)
        {
            _on_enter();
        }
    };

    void on_exit()
    {
        if (_on_exit != nullptr)
        {
            _on_exit();
        }
    };

    std::vector<state_transition> _transitions;

    std::function<void()> _on_enter    = nullptr;
    std::function<void()> _on_exit     = nullptr;
    std::function<void(float)> _update = nullptr;

    friend class state_machine;
};

class state_machine {
   public:
    state_machine(std::shared_ptr<state> initial_state) :
        _current_state(initial_state) { assert(_current_state != nullptr); };

    void start()
    {
        if (_current_state == nullptr)
            return;

        _current_state->on_enter();
    }

    void update(float delta_time)
    {
        if (_current_state == nullptr)
            return;

        std::shared_ptr<state> next_state = nullptr;

        _current_state->update(delta_time);
        _current_state->evaluate_transitions(next_state);

        if (next_state != nullptr)
        {
            _current_state = next_state;
        }
    }

    void stop()
    {
        if (_current_state == nullptr)
            return;

        _current_state->on_exit();
    }

   protected:
    std::shared_ptr<state> _current_state = nullptr;
};

#endif // STATE_HPP
