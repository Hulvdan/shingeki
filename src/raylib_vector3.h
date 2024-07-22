#include "raymath.h"

Vector3  operator+(const Vector3& a);
Vector3  operator-(const Vector3& a);
Vector3  operator+(const Vector3& a, const Vector3& b);
Vector3  operator-(const Vector3& a, const Vector3& b);
Vector3  operator*(const Vector3& a, const float scale);
Vector3  operator*(const Vector3& a, const Vector3& b);
Vector3  operator/(const Vector3& a, const float scale);
Vector3  operator/(const Vector3& a, const Vector3& b);
Vector3& operator+=(Vector3& a, const Vector3& b);
Vector3& operator-=(Vector3& a, const Vector3& b);
Vector3& operator*=(Vector3& a, const Vector3& b);
Vector3& operator*=(Vector3& a, const float scale);
Vector3& operator/=(Vector3& a, const Vector3& b);
Vector3& operator/=(Vector3& a, const float scale);
