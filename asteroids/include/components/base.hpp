#ifndef BASE_DEFINITIONS_HPP
#define BASE_DEFINITIONS_HPP

#include <raylib.h>

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
};

#endif // BASE_DEFINITIONS_HPP
