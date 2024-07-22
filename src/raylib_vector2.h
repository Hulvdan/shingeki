#include "raymath.h"

Vector2  operator+(const Vector2& a);
Vector2  operator-(const Vector2& a);
Vector2  operator+(const Vector2& a, const Vector2& b);
Vector2  operator-(const Vector2& a, const Vector2& b);
Vector2  operator*(const Vector2& a, const float scale);
Vector2  operator*(const Vector2& a, const Vector2& b);
Vector2  operator/(const Vector2& a, const float scale);
Vector2  operator/(const Vector2& a, const Vector2& b);
Vector2& operator+=(Vector2& a, const Vector2& b);
Vector2& operator-=(Vector2& a, const Vector2& b);
Vector2& operator*=(Vector2& a, const Vector2& b);
Vector2& operator*=(Vector2& a, const float scale);
Vector2& operator/=(Vector2& a, const Vector2& b);
Vector2& operator/=(Vector2& a, const float scale);
