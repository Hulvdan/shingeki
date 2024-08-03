// Vector4 operator+(const Vector4& a) {
//     return Vector4(a.x, a.y, a.z);
// }
// Vector4 operator-(const Vector4& a) {
//     return Vector4(-a.x, -a.y, -a.z);
// }
// Vector4 operator+(const Vector4& a, const Vector4& b) {
//     return Vector3Add(a, b);
// }
// Vector4 operator-(const Vector4& a, const Vector4& b) {
//     return Vector3Subtract(a, b);
// }
Vector4 operator*(const Vector4& a, const float scale) {
    Vector4 b = a;
    b.x *= scale;
    b.y *= scale;
    b.z *= scale;
    b.w *= scale;
    return b;
}
// Vector4 operator*(const Vector4& a, const Vector4& b) {
//     return Vector3Multiply(a, b);
// }
// Vector4 operator/(const Vector4& a, const float scale) {
//     return Vector4(a.x / scale, a.y / scale, a.z / scale);
// }
// Vector4 operator/(const Vector4& a, const Vector4& b) {
//     return Vector4(a.x / b.x, a.y / b.y, a.z / b.z);
// }
Vector4& operator+=(Vector4& a, const Vector4& b) {
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    a.w += b.w;
    return a;
}
Vector4& operator-=(Vector4& a, const Vector4& b) {
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    a.w -= b.w;
    return a;
}
// Vector4& operator*=(Vector4& a, const Vector4& b) {
//     a.x *= b.x;
//     a.y *= b.y;
//     a.z *= b.z;
//     return a;
// }
Vector4& operator*=(Vector4& a, const float scale) {
    a.x *= scale;
    a.y *= scale;
    a.z *= scale;
    a.w *= scale;
    return a;
}
// Vector4& operator/=(Vector4& a, const Vector4& b) {
//     a.x /= b.x;
//     a.y /= b.y;
//     a.z /= b.z;
//     return a;
// }
// Vector4& operator/=(Vector4& a, const float scale) {
//     a.x /= scale;
//     a.y /= scale;
//     a.z /= scale;
//     return a;
// }

constexpr Vector4 Vector4Up       = Vector4(0, 1, 0, 0);
constexpr Vector4 Vector4Down     = Vector4(0, -1, 0, 0);
constexpr Vector4 Vector4Right    = Vector4(1, 0, 0, 0);
constexpr Vector4 Vector4Left     = Vector4(-1, 0, 0, 0);
constexpr Vector4 Vector4Forward  = Vector4(0, 0, 1, 0);
constexpr Vector4 Vector4Backward = Vector4(0, 0, -1, 0);
