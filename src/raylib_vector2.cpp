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
