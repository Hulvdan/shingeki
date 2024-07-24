float MoveTowards(float v, float target, float maxDistance) {
    Assert(maxDistance >= 0);

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

TEST_CASE ("MoveTowards") {
    Assert(FloatEquals(MoveTowards(0, 10, 5), 5));
    Assert(FloatEquals(MoveTowards(-10, 10, 5), -5));
    Assert(FloatEquals(MoveTowards(10, -10, 5), 5));
    Assert(FloatEquals(MoveTowards(0, 10, 30), 10));
    Assert(FloatEquals(MoveTowards(-10, 10, 30), 10));
    Assert(FloatEquals(MoveTowards(10, -10, 30), -10));
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

int Floor(int number, int factor) {
    int result = (number / factor) * factor;
    if (number < 0 && number % factor != 0) {
        result -= factor;
    }
    return result;
}

TEST_CASE ("Floor") {
    Assert(Floor(0, 20) == 0);
    Assert(Floor(1, 20) == 0);
    Assert(Floor(-1, 20) == -20);
    Assert(Floor(20, 20) == 20);
    Assert(Floor(21, 20) == 20);
}

int CeilDivision(int value, int divisor) {
    Assert(value >= 0);
    Assert(divisor > 0);
    return value / divisor + ((value % divisor) != 0);
}

TEST_CASE ("CeilDivision") {
    Assert(CeilDivision(10, 1) == 10);
    Assert(CeilDivision(10, 5) == 2);
    Assert(CeilDivision(10, 6) == 2);
    Assert(CeilDivision(10, 4) == 3);
}
