Vector3 ToVector3(const float scalar) {
    Vector3 result(scalar, scalar, scalar);

    return result;
}

Vector3 ToVector3(const Vector2 value) {
    Vector3 result(value.x, value.y, 0);

    return result;
}

Vector3 ToVector3(const Vector4 value) {
    Vector3 result(value.x, value.y, value.z);

    return result;
}

Vector3 operator+(const Vector3& v) {
    Vector3 result(v.x, v.y, v.z);

    return result;
}

Vector3 operator-(const Vector3& v) {
    Vector3 result(-v.x, -v.y, -v.z);

    return result;
}

Vector3 operator+(const Vector3& v1, const Vector3& v2) {
    Vector3 result = Vector3Add(v1, v2);

    return result;
}

Vector3 operator-(const Vector3& v1, const Vector3& v2) {
    Vector3 result = Vector3Subtract(v1, v2);

    return result;
}

Vector3 operator*(const Vector3& v, const float scale) {
    Vector3 result = Vector3Scale(v, scale);

    return result;
}

Vector3 operator*(const Vector3& v1, const Vector3& v2) {
    Vector3 result = Vector3Multiply(v1, v2);

    return result;
}

Vector3 operator/(const Vector3& v, const float scale) {
    Vector3 result = Vector3Scale(v, 1.0f / scale);

    return result;
}

Vector3 operator/(const Vector3& v1, const Vector3& v2) {
    Vector3 result = Vector3Divide(v1, v2);

    return result;
}

Vector3& operator+=(Vector3& v1, const Vector3& v2) {
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;

    return v1;
}

Vector3& operator-=(Vector3& v1, const Vector3& v2) {
    v1.x -= v2.x;
    v1.y -= v2.y;
    v1.z -= v2.z;

    return v1;
}

Vector3& operator*=(Vector3& v1, const Vector3& v2) {
    v1.x *= v2.x;
    v1.y *= v2.y;
    v1.z *= v2.z;

    return v1;
}

Vector3& operator*=(Vector3& v, const float scale) {
    v.x *= scale;
    v.y *= scale;
    v.z *= scale;

    return v;
}

Vector3& operator/=(Vector3& v1, const Vector3& v2) {
    v1.x /= v2.x;
    v1.y /= v2.y;
    v1.z /= v2.z;

    return v1;
}

Vector3& operator/=(Vector3& v, const float scale) {
    v.x /= scale;
    v.y /= scale;
    v.z /= scale;

    return v;
}

constexpr Vector3 Vector3Up       = Vector3(0, 1, 0);
constexpr Vector3 Vector3Down     = Vector3(0, -1, 0);
constexpr Vector3 Vector3Right    = Vector3(1, 0, 0);
constexpr Vector3 Vector3Left     = Vector3(-1, 0, 0);
constexpr Vector3 Vector3Forward  = Vector3(0, 0, 1);
constexpr Vector3 Vector3Backward = Vector3(0, 0, -1);
