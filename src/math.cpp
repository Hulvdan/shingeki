float MoveTowards(float v, float target, float maxDistance) {
    assert(maxDistance >= 0);

    if (v > target) {
        v -= maxDistance;
        if (v < target)
            v = target;
    }
    else if (v < target) {
        v += maxDistance;
        if (v > target)
            v = target;
    }

    return v;
}

// ExponentialDecay - это ~Lerp, но без зависимости от framerate-а.
// NOTE: https://www.youtube.com/watch?v=LSNQuFEDOyQ
float ExponentialDecay(float a, float b, float decay, float dt) {
    return b + (a - b) * expf(-decay * dt);
}

// ExponentialDecay для Vector2.
Vector2 Vector2ExponentialDecay(Vector2 a, Vector2 b, float decay, float dt) {
    return b + (a - b) * expf(-decay * dt);
}

// Usage:
//
//     Floor(0, 20) == 0
//     Floor(1, 20) == 0
//     Floor(-1, 20) == -20
//     Floor(20, 20) == 20
//     Floor(21, 20) == 20
//
int Floor(int number, int factor) {
    int result = (number / factor) * factor;
    if (number < 0 && number % factor != 0) {
        result -= factor;
    }
    return result;
}

// Usage:
//
//     CeilDivision(10, 1) = 10
//     CeilDivision(10, 5) = 2
//     CeilDivision(10, 6) = 2
//     CeilDivision(10, 4) = 3
//
int CeilDivision(int value, int divisor) {
    assert(value >= 0);
    assert(divisor > 0);
    return value / divisor + ((value % divisor) != 0);
}
