#include <math.hpp>

Vector2 operator+(const Vector2& lhs, const Vector2& rhs)
{
    return Vector2Add(lhs, rhs);
}

Vector2 operator-(const Vector2& lhs, const Vector2& rhs)
{
    return Vector2Subtract(lhs, rhs);
}

Vector2 operator*(const float& lhs, const Vector2& rhs)
{
    return Vector2Scale(rhs, lhs);
}

Vector2 operator*(const Vector2& lhs, const float& rhs)
{
    return Vector2Scale(lhs, rhs);
}
