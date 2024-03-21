#ifndef BASE_DEFINITIONS_HPP
#define BASE_DEFINITIONS_HPP

#include <raylib.h>

#include <cstddef>
#include <cstdint>
#include <entt/entt.hpp>
#include <functional>
#include <string_view>

using entt::operator""_hs;

struct transform
{
    Vector2 position;
    Vector2 direction;
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

enum class GAME_TEXTURES : std::uint32_t
{
    MAINTEXTURE  = "MAINTEX"_hs,
    PLANETEXTURE = "EXPTEX"_hs

};
#endif // BASE_DEFINITIONS_HPP
