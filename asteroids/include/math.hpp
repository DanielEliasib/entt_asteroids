#ifndef MATH_HPP
#define MATH_HPP

#include <raylib.h>
#include <raymath.h>

// Add overload for Vector2 + Vector2
Vector2 operator+(const Vector2& lhs, const Vector2& rhs)
{
    return Vector2Add(lhs, rhs);
}

#endif // MATH_HPP
