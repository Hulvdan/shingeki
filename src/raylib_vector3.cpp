Vector3 ToVector3(Vector2 value) {
    Vector3 result = {value.x, value.y, 0};

    return result;
}

Vector3 ToVector3(Vector4 value) {
    Vector3 result = {value.x, value.y, value.z};

    return result;
}

Vector3 operator+(const Vector3& a) {
    Vector3 result = {a.x, a.y, a.z};

    return result;
}

Vector3 operator-(const Vector3& a) {
    Vector3 result = {-a.x, -a.y, -a.z};

    return result;
}

Vector3 operator+(const Vector3& a, const Vector3& b) {
    Vector3 result = Vector3Add(a, b);

    return result;
}

Vector3 operator-(const Vector3& a, const Vector3& b) {
    Vector3 result = Vector3Subtract(a, b);

    return result;
}

Vector3 operator*(const Vector3& a, const float scale) {
    Vector3 result = Vector3Scale(a, scale);

    return result;
}

Vector3 operator*(const Vector3& a, const Vector3& b) {
    Vector3 result = Vector3Multiply(a, b);

    return result;
}

Vector3 operator/(const Vector3& a, const float scale) {
    Vector3 result = {a.x / scale, a.y / scale, a.z / scale};

    return result;
}

Vector3 operator/(const Vector3& a, const Vector3& b) {
    Vector3 result = {a.x / b.x, a.y / b.y, a.z / b.z};

    return result;
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
