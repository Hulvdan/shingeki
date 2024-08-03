Vector4 Vector4Add(const Vector4 v1, const Vector4 v2) {
    Vector4 result = {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w};

    return result;
};

Vector4 Vector4Subtract(const Vector4 v1, const Vector4 v2) {
    Vector4 result = {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w};

    return result;
}

Vector4 Vector4Multiply(const Vector4 v1, const Vector4 v2) {
    Vector4 result = {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w};

    return result;
}

Vector4 Vector4Scale(const Vector4 v, const float scalar) {
    Vector4 result = {v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar};

    return result;
}

Vector4 ToVector4(const Vector2 v) {
    Vector4 result = {v.x, v.y, 0, 0};

    return result;
}

Vector4 ToVector4(const Vector3 v) {
    Vector4 result = {v.x, v.y, v.z, 0};

    return result;
}

Vector4 operator+(const Vector4& v) {
    Vector4 result = {v.x, v.y, v.z, v.w};

    return result;
}

Vector4 operator-(const Vector4& v) {
    Vector4 result = {-v.x, -v.y, -v.z, -v.w};

    return result;
}

Vector4 operator+(const Vector4& v1, const Vector4& v2) {
    Vector4 result = {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w};

    return result;
}

Vector4 operator-(const Vector4& v1, const Vector4& v2) {
    Vector4 result = {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w};

    return result;
}

Vector4 operator*(const Vector4& v, float scalar) {
    Vector4 result = {v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar};

    return result;
}

Vector4 operator*(const Vector4& v1, const Vector4& v2) {
    Vector4 result = {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w};

    return result;
}

Vector4 operator/(const Vector4& v, const float scalar) {
    Vector4 result = {v.x / scalar, v.y / scalar, v.z / scalar, v.w / scalar};

    return result;
}

Vector4 operator/(const Vector4& v1, const Vector4& v2) {
    Vector4 result = {v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w};

    return result;
}

Vector4& operator+=(Vector4& v1, const Vector4& v2) {
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;
    v1.w += v2.w;

    return v1;
}

Vector4& operator-=(Vector4& v1, const Vector4& v2) {
    v1.x -= v2.x;
    v1.y -= v2.y;
    v1.z -= v2.z;
    v1.w -= v2.w;

    return v1;
}

Vector4& operator*=(Vector4& v1, const Vector4& v2) {
    v1.x *= v2.x;
    v1.y *= v2.y;
    v1.z *= v2.z;
    v1.w *= v2.w;

    return v1;
}

Vector4& operator*=(Vector4& v, const float scalar) {
    v.x *= scalar;
    v.y *= scalar;
    v.z *= scalar;
    v.w *= scalar;

    return v;
}

Vector4& operator/=(Vector4& v1, const Vector4& v2) {
    v1.x /= v2.x;
    v1.y /= v2.y;
    v1.z /= v2.z;
    v1.w /= v2.w;

    return v1;
}

Vector4& operator/=(Vector4& v, const float scalar) {
    v.x /= scalar;
    v.y /= scalar;
    v.z /= scalar;
    v.w /= scalar;

    return v;
}

constexpr Vector4 Vector4Up       = Vector4(0, 1, 0, 0);
constexpr Vector4 Vector4Down     = Vector4(0, -1, 0, 0);
constexpr Vector4 Vector4Right    = Vector4(1, 0, 0, 0);
constexpr Vector4 Vector4Left     = Vector4(-1, 0, 0, 0);
constexpr Vector4 Vector4Forward  = Vector4(0, 0, 1, 0);
constexpr Vector4 Vector4Backward = Vector4(0, 0, -1, 0);
