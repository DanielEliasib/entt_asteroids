#ifndef MATH_HPP
#define MATH_HPP

#include <raylib.h>
#include <raymath.h>

inline Vector2 operator+(const Vector2& lhs, const Vector2& rhs)
{
    return Vector2Add(lhs, rhs);
}

inline Vector2 operator-(const Vector2& lhs, const Vector2& rhs)
{
    return Vector2Subtract(lhs, rhs);
}

inline Vector2 operator*(const float& lhs, const Vector2& rhs)
{
    return Vector2Scale(rhs, lhs);
}

inline Vector2 operator*(const Vector2& lhs, const float& rhs)
{
    return Vector2Scale(lhs, rhs);
}

#endif // MATH_HPP
