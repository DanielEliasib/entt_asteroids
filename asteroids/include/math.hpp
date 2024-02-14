#ifndef MATH_HPP
#define MATH_HPP

#include <raymath.h>

Vector2 operator+(const Vector2& lhs, const Vector2& rhs);

Vector2 operator-(const Vector2& lhs, const Vector2& rhs);

Vector2 operator*(const float& lhs, const Vector2& rhs);

Vector2 operator*(const Vector2& lhs, const float& rhs);

#endif // MATH_HPP
