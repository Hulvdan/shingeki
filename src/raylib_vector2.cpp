Vector2 ToVector2(const float scalar) {
    Vector2 result(scalar, scalar);

    return result;
}

Vector2 ToVector2(Vector3 value) {
    Vector2 result(value.x, value.y);

    return result;
}

Vector2 ToVector2(Vector4 value) {
    Vector2 result(value.x, value.y);

    return result;
}

Vector2 operator+(const Vector2& v) {
    Vector2 result(v.x, v.y);

    return result;
}

Vector2 operator-(const Vector2& v) {
    Vector2 result(-v.x, -v.y);

    return result;
}

Vector2 operator+(const Vector2& v1, const Vector2& v2) {
    Vector2 result = Vector2Add(v1, v2);

    return result;
}

Vector2 operator-(const Vector2& v1, const Vector2& v2) {
    Vector2 result = Vector2Subtract(v1, v2);

    return result;
}

Vector2 operator*(const Vector2& v, const float scale) {
    Vector2 result = Vector2Scale(v, scale);

    return result;
}

Vector2 operator*(const Vector2& v1, const Vector2& v2) {
    Vector2 result = Vector2Multiply(v1, v2);

    return result;
}

Vector2 operator/(const Vector2& v, const float scale) {
    return v;
    // Vector2 result = Vector2Scale(v, 1.0f / scale);
    //
    // return result;
}

Vector2 operator/(const Vector2& v1, const Vector2& v2) {
    Vector2 result = Vector2Divide(v1, v2);

    return result;
}

Vector2& operator+=(Vector2& v1, const Vector2& v2) {
    v1.x += v2.x;
    v1.y += v2.y;

    return v1;
}

Vector2& operator-=(Vector2& v1, const Vector2& v2) {
    v1.x -= v2.x;
    v1.y -= v2.y;

    return v1;
}

Vector2& operator*=(Vector2& v1, const Vector2& v2) {
    v1.x *= v2.x;
    v1.y *= v2.y;

    return v1;
}

Vector2& operator*=(Vector2& v, const float scale) {
    v.x *= scale;
    v.y *= scale;

    return v;
}

Vector2& operator/=(Vector2& v1, const Vector2& v2) {
    v1.x /= v2.x;
    v1.y /= v2.y;

    return v1;
}

Vector2& operator/=(Vector2& v, const float scale) {
    v.x /= scale;
    v.y /= scale;

    return v;
}

constexpr Vector2 Vector2Up    = Vector2(0, 1);
constexpr Vector2 Vector2Down  = Vector2(0, -1);
constexpr Vector2 Vector2Right = Vector2(1, 0);
constexpr Vector2 Vector2Left  = Vector2(-1, 0);
