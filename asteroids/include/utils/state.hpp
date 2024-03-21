#ifndef STATE_HPP
#define STATE_HPP

#include <cassert>
#include <functional>
#include <memory>
#include <unordered_map>

class state;

class state_transition {
   public:
    state_transition(std::function<bool()> condition, std::shared_ptr<state> target_state) :
        _condition(condition),
        _target_state(target_state)
    {
        assert(_condition != nullptr);
        assert(_target_state != nullptr);
    };

    bool evaluate() const { return _condition(); };
    std::shared_ptr<state> target_state() const { return _target_state; };

   protected:
    std::function<bool()> _condition     = nullptr;
    std::shared_ptr<state> _target_state = nullptr;
};

class state {
   public:
    state(std::function<void()> on_enter, std::function<void()> on_exit, std::function<void()> update) :
        _on_enter(on_enter),
        _on_exit(on_exit),
        _update(update){};

    void add_transition(std::function<bool()> condition, std::shared_ptr<state> target_state)
    {
        _transitions.push_back(state_transition(condition, target_state));
    }

   protected:
    void update()
    {
        if (_update != nullptr)
        {
            _update();
        }
    };

    bool evaluate_transitions(std::shared_ptr<state>& next_state)
    {
        for (auto& transition : _transitions)
        {
            if (!transition.evaluate())
                continue;

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

    std::function<void()> _on_enter = nullptr;
    std::function<void()> _on_exit  = nullptr;
    std::function<void()> _update   = nullptr;

    friend class state_machine;
};

class state_machine {
   public:
    state_machine(std::shared_ptr<state> initial_state) :
        _current_state(initial_state) { assert(_current_state != nullptr); };

    void update()
    {
        std::shared_ptr<state> next_state = nullptr;

        if (_current_state != nullptr)
        {
            _current_state->update();
            _current_state->evaluate_transitions(next_state);
        }

        if (next_state != nullptr)
        {
            _current_state = next_state;
        }
    }

   protected:
    std::shared_ptr<state> _current_state = nullptr;
};

#endif // STATE_HPP
