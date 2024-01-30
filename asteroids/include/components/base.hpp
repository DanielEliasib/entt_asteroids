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

	float drag;

    Vector2 external_force;
    Vector2 external_impulse;
};

struct lifetime
{
	float radius;
};

#endif // BASE_DEFINITIONS_HPP
