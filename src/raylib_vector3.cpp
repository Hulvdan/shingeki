/**********************************************************************************************
 *
 *   raylib - Advance Game template
 *
 *   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
 *
 *   This software is provided "as-is", without any express or implied warranty. In no
 *event will the authors be held liable for any damages arising from the use of this
 *software.
 *
 *   Permission is granted to anyone to use this software for any purpose, including
 *commercial applications, and to alter it and redistribute it freely, subject to the
 *following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not claim that
 *you wrote the original software. If you use this software in a product, an
 *acknowledgment in the product documentation would be appreciated but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not be
 *misrepresented as being the original software.
 *
 *     3. This notice may not be removed or altered from any source distribution.
 *
 **********************************************************************************************/

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

constexpr Vector3 Vector3Up       = Vector3(0, 1, 0);
constexpr Vector3 Vector3Down     = Vector3(0, -1, 0);
constexpr Vector3 Vector3Right    = Vector3(1, 0, 0);
constexpr Vector3 Vector3Left     = Vector3(-1, 0, 0);
constexpr Vector3 Vector3Forward  = Vector3(0, 0, 1);
constexpr Vector3 Vector3Backward = Vector3(0, 0, -1);
