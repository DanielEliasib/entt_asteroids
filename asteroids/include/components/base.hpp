#ifndef BASE_DEFINITIONS_HPP
#define BASE_DEFINITIONS_HPP

#include <raylib.h>

#include <cstdint>
#include <entt/entt.hpp>
#include <functional>

using entt::operator""_hs;

struct transform
{
    Vector2 position;
    float rotation;
};

struct physics
{
    Vector2 velocity;
    float angular_velocity;

    float drag;

    Vector2 external_force;
    Vector2 external_impulse;
};

struct lifetime
{
    float lifetime;
    float elapsed;

    std::function<void(entt::registry&)> on_end = nullptr;
};

static const std::uint32_t kill_tag = "KILL"_hs;

enum struct team
{
    PLAYER,
    ENEMY,
};

enum class GAME_TEXTURES : std::uint32_t
{
    MAINTEXTURE  = "MAINTEX"_hs,
    PLANETEXTURE = "EXPTEX"_hs

};
#endif // BASE_DEFINITIONS_HPP
