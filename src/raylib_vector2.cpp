#include "raymath.h"

Vector2 operator+(const Vector2& a) {
    return Vector2(a.x, a.y);
}
Vector2 operator-(const Vector2& a) {
    return Vector2(-a.x, -a.y);
}
Vector2 operator+(const Vector2& a, const Vector2& b) {
    return Vector2Add(a, b);
}
Vector2 operator-(const Vector2& a, const Vector2& b) {
    return Vector2Subtract(a, b);
}
Vector2 operator*(const Vector2& a, const float scale) {
    return Vector2Scale(a, scale);
}
Vector2 operator*(const Vector2& a, const Vector2& b) {
    return Vector2Multiply(a, b);
}
Vector2 operator/(const Vector2& a, const float scale) {
    return Vector2(a.x / scale, a.y / scale);
}
Vector2 operator/(const Vector2& a, const Vector2& b) {
    return Vector2(a.x / b.x, a.y / b.y);
}
Vector2& operator+=(Vector2& a, const Vector2& b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}
Vector2& operator-=(Vector2& a, const Vector2& b) {
    a.x -= b.x;
    a.y -= b.y;
    return a;
}
Vector2& operator*=(Vector2& a, const Vector2& b) {
    a.x *= b.x;
    a.y *= b.y;
    return a;
}
Vector2& operator*=(Vector2& a, const float scale) {
    a.x *= scale;
    a.y *= scale;
    return a;
}
Vector2& operator/=(Vector2& a, const Vector2& b) {
    a.x /= b.x;
    a.y /= b.y;
    return a;
}
Vector2& operator/=(Vector2& a, const float scale) {
    a.x /= scale;
    a.y /= scale;
    return a;
}
