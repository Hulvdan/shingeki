#include "raymath.h"

Vector3 operator+(const Vector3& a) {
    return Vector3(a.x, a.y, a.z);
}
Vector3 operator-(const Vector3& a) {
    return Vector3(-a.x, -a.y, -a.z);
}
Vector3 operator+(const Vector3& a, const Vector3& b) {
    return Vector3Add(a, b);
}
Vector3 operator-(const Vector3& a, const Vector3& b) {
    return Vector3Subtract(a, b);
}
Vector3 operator*(const Vector3& a, const float scale) {
    return Vector3Scale(a, scale);
}
Vector3 operator*(const Vector3& a, const Vector3& b) {
    return Vector3Multiply(a, b);
}
Vector3 operator/(const Vector3& a, const float scale) {
    return Vector3(a.x / scale, a.y / scale, a.z / scale);
}
Vector3 operator/(const Vector3& a, const Vector3& b) {
    return Vector3(a.x / b.x, a.y / b.y, a.z / b.z);
}
Vector3& operator+=(Vector3& a, const Vector3& b) {
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}
Vector3& operator-=(Vector3& a, const Vector3& b) {
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
}
Vector3& operator*=(Vector3& a, const Vector3& b) {
    a.x *= b.x;
    a.y *= b.y;
    a.z *= b.z;
    return a;
}
Vector3& operator*=(Vector3& a, const float scale) {
    a.x *= scale;
    a.y *= scale;
    a.z *= scale;
    return a;
}
Vector3& operator/=(Vector3& a, const Vector3& b) {
    a.x /= b.x;
    a.y /= b.y;
    a.z /= b.z;
    return a;
}
Vector3& operator/=(Vector3& a, const float scale) {
    a.x /= scale;
    a.y /= scale;
    a.z /= scale;
    return a;
}
