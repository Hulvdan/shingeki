Vector2 ToVector2(Vector3 value) {
    Vector2 result = {value.x, value.y};

    return result;
}

Vector2 ToVector2(Vector4 value) {
    Vector2 result = {value.x, value.y};

    return result;
}

Vector2 operator+(const Vector2& a) {
    Vector2 result = {a.x, a.y};

    return result;
}

Vector2 operator-(const Vector2& a) {
    Vector2 result = {-a.x, -a.y};

    return result;
}

Vector2 operator+(const Vector2& a, const Vector2& b) {
    Vector2 result = Vector2Add(a, b);

    return result;
}

Vector2 operator-(const Vector2& a, const Vector2& b) {
    Vector2 result = Vector2Subtract(a, b);

    return result;
}

Vector2 operator*(const Vector2& a, const float scale) {
    Vector2 result = Vector2Scale(a, scale);

    return result;
}

Vector2 operator*(const Vector2& a, const Vector2& b) {
    Vector2 result = Vector2Multiply(a, b);

    return result;
}

Vector2 operator/(const Vector2& a, const float scale) {
    Vector2 result = {a.x / scale, a.y / scale};

    return result;
}

Vector2 operator/(const Vector2& a, const Vector2& b) {
    Vector2 result = {a.x / b.x, a.y / b.y};

    return result;
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

constexpr Vector2 Vector2Up    = Vector2(0, 1);
constexpr Vector2 Vector2Down  = Vector2(0, -1);
constexpr Vector2 Vector2Right = Vector2(1, 0);
constexpr Vector2 Vector2Left  = Vector2(-1, 0);
