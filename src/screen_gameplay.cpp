static constexpr int fpsValues[] = {60, 20, 40};

//----------------------------------------------------------------------------------
// Module Variables Definition (local).
//----------------------------------------------------------------------------------

globalVar struct {
    int  currentFPSValueIndex = 0;
    bool gizmosEnabled        = true;

    int finishScreen = 0;

    float   rotationY;
    float   rotationHorizontal;
    float   playerSpeed = 10.0f;
    Vector3 lookingDirection;

    Camera3D camera         = {};
    Vector3  playerPosition = {0.0f, 2.0f, 10.0f};
} gdata;

//----------------------------------------------------------------------------------
// Helper Functions Definition.
//----------------------------------------------------------------------------------

Vector2 GetRectangleSize(Rectangle rect) {
    return {rect.width, rect.height};
}

//----------------------------------------------------------------------------------
// Gameplay Functions Definition.
//----------------------------------------------------------------------------------
Vector2 GetPlayerMovementControlVector() {
    Vector2 result = {};

    bool  move      = true;
    float direction = 0;

    if (IsKeyDown(KEY_A))
        result.x -= 1;
    if (IsKeyDown(KEY_D))
        result.x += 1;
    if (IsKeyDown(KEY_W))
        result.y -= 1;
    if (IsKeyDown(KEY_S))
        result.y += 1;

    result = Vector2Normalize(result);
    return result;
}

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition.
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic.
void InitGameplayScreen() {
    // Tests
    // ------------------------------------------------------------
    assert(Floor(0, 20) == 0);
    assert(Floor(1, 20) == 0);
    assert(Floor(-1, 20) == -20);
    assert(Floor(20, 20) == 20);
    assert(Floor(21, 20) == 20);
    assert(CeilDivision(10, 1) == 10);
    assert(CeilDivision(10, 5) == 2);
    assert(CeilDivision(10, 6) == 2);
    assert(CeilDivision(10, 4) == 3);
    // ------------------------------------------------------------

    gdata.finishScreen = 0;

    gdata.camera.target     = Vector3{0.0f, 0.0f, 0.0f};
    gdata.camera.up         = Vector3{0.0f, 1.0f, 0.0f};
    gdata.camera.fovy       = 45.0f;
    gdata.camera.projection = CAMERA_PERSPECTIVE;

    DisableCursor();
}

// Gameplay Screen Update logic.
void UpdateGameplayScreen() {
    auto dt = GetFrameTime();

    // Press enter or tap to change to ENDING screen.
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
        gdata.finishScreen = 1;

    {  // Controlling FPS.
        if (IsKeyPressed(KEY_F1)) {
            gdata.currentFPSValueIndex++;
            if (gdata.currentFPSValueIndex
                >= sizeof(fpsValues) / sizeof(gdata.currentFPSValueIndex))
                gdata.currentFPSValueIndex = 0;
            SetTargetFPS(fpsValues[gdata.currentFPSValueIndex]);
        }
    }

    {  // Camera rotation.
        const float sensitivity = 1.0f / 300.0f;

        const auto delta = GetMouseDelta();

        // verticalRotationBorder - Ограничение для того, чтобы,
        // поднимая камеру вверх, мы не начали смотреть перевёрнуто себе за спину.
        const auto verticalRotationBorder = PI / 2 - 0.1f;
        gdata.rotationY -= delta.y * sensitivity;
        gdata.rotationY
            = Clamp(gdata.rotationY, -verticalRotationBorder, verticalRotationBorder);

        gdata.rotationHorizontal -= delta.x * sensitivity;
        if (gdata.rotationHorizontal > 2 * PI)
            gdata.rotationHorizontal -= 2 * PI;
        if (gdata.rotationHorizontal < 2 * PI)
            gdata.rotationHorizontal += 2 * PI;

        auto direction = Vector3(1, 0, 0);
        direction
            = Vector3RotateByAxisAngle(direction, Vector3(0, 0, 1), gdata.rotationY);
        direction = Vector3RotateByAxisAngle(
            direction, Vector3(0, 1, 0), gdata.rotationHorizontal
        );

        gdata.lookingDirection = direction;
    }

    {  // Player Movement.
        const Vector2 lookingHorizontalDirection
            = {gdata.lookingDirection.x, gdata.lookingDirection.z};

        const auto controlVector = GetPlayerMovementControlVector();

        if (controlVector.x != 0 || controlVector.y != 0) {
            const auto angle = atan2f(controlVector.y, controlVector.x);

            const auto dHoriz = Vector2Rotate(lookingHorizontalDirection, PI / 2 + angle)
                                * (dt * gdata.playerSpeed);

            gdata.playerPosition += Vector3(dHoriz.x, 0, dHoriz.y);
            // gdata.playerPosition
            //     += Vector3RotateByAxisAngle(Vector3(dt, 0, 0), Vector3(0, 1, 0),
            //     direction);
        }
    }

    {  // Enabling drawing gizmos.
        if (IsKeyPressed(KEY_F2))
            gdata.gizmosEnabled = !gdata.gizmosEnabled;
    }
}

// Gameplay Screen Draw logic.
void DrawGameplayScreen() {
    DebugTextReset();

    const auto    screenWidth  = GetScreenWidth();
    const auto    screenHeight = GetScreenHeight();
    const Vector2 screenSize{(float)screenWidth, (float)screenHeight};

    DrawRectangle(0, 0, screenWidth, screenHeight, BLACK);

    auto& camera    = gdata.camera;
    camera.position = gdata.playerPosition;
    camera.target   = camera.position + gdata.lookingDirection * 100.0f;

    BeginMode3D(camera);

    {  // Drawing world.
        struct {
            Vector3 pos;
            Vector3 size;
            Color   color;
            Color   wireColor;
        } cubes[] = {
            {{11, 3, 8}, {2, 6, 2}, GREEN, MAROON},
            {{16, 3, 3}, {2, 6, 2}, BLUE, MAROON},
            {{-15, 4, 11}, {2, 8, 2}, YELLOW, MAROON},
            {{3, 2, -10}, {2, 4, 2}, RED, MAROON},
            {{19, 1, 13}, {2, 2, 2}, MAGENTA, MAROON},
            {{6, 3, 13}, {2, 6, 2}, MAGENTA, MAROON},
            {{11, 6, 16}, {1, 12, 1}, GRAY, MAROON},
        };

        int cubesCount = sizeof(cubes) / sizeof(cubes[0]);

        FOR_RANGE (int, i, cubesCount) {
            const auto  cube = cubes[i];
            const auto& pos  = cube.pos;
            const auto& size = cube.size;

            DrawCube(pos, size.x, size.y, size.z, cube.color);
            DrawCubeWires(pos, size.x, size.y, size.z, cube.wireColor);
            DrawGrid(100, 1.0f);
        }
    }
    if (gdata.gizmosEnabled) {
    }
    EndMode3D();

    DebugTextDraw(
        TextFormat("FPS: %i (press F1 to change)", fpsValues[gdata.currentFPSValueIndex])
    );
    DebugTextDraw(TextFormat("Toggle gizmos - F2"));
}

// Gameplay Screen Unload logic.
void UnloadGameplayScreen() {
    EnableCursor();
}

// Gameplay Screen should finish?
int FinishGameplayScreen() {
    return gdata.finishScreen;
}
