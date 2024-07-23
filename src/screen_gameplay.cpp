static constexpr int fpsValues[] = {60, 20, 40};

//----------------------------------------------------------------------------------
// Module Variables Definition (local).
//----------------------------------------------------------------------------------

globalVar struct {
    int  currentFPSValueIndex = 0;
    bool gizmosEnabled        = true;

    int finishScreen = 0;

    Camera3D camera = {};
} gdata;

globalVar struct {
    bool isGrounded;

    float   rotationY;
    float   rotationHorizontal;
    Vector3 lookingDirection;

    Vector3 playerPosition = {0.0f, 2.0f, 10.0f};

    const float playerSpeed = 10.0f;
} gplayer;

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
        gplayer.rotationY -= delta.y * sensitivity;
        gplayer.rotationY
            = Clamp(gplayer.rotationY, -verticalRotationBorder, verticalRotationBorder);

        gplayer.rotationHorizontal -= delta.x * sensitivity;
        if (gplayer.rotationHorizontal > 2 * PI)
            gplayer.rotationHorizontal -= 2 * PI;
        if (gplayer.rotationHorizontal < 2 * PI)
            gplayer.rotationHorizontal += 2 * PI;

        auto direction = Vector3(1, 0, 0);
        direction
            = Vector3RotateByAxisAngle(direction, Vector3(0, 0, 1), gplayer.rotationY);
        direction = Vector3RotateByAxisAngle(
            direction, Vector3(0, 1, 0), gplayer.rotationHorizontal
        );

        gplayer.lookingDirection = direction;
    }

    {  // Player Movement.
        const Vector2 lookingHorizontalDirection
            = {gplayer.lookingDirection.x, gplayer.lookingDirection.z};

        const auto controlVector = GetPlayerMovementControlVector();

        if (controlVector.x != 0 || controlVector.y != 0) {
            const auto angle = atan2f(controlVector.y, controlVector.x);

            const auto dHoriz = Vector2Rotate(lookingHorizontalDirection, PI / 2 + angle)
                                * (dt * gplayer.playerSpeed);

            gplayer.playerPosition += Vector3(dHoriz.x, 0, dHoriz.y);
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
    camera.position = gplayer.playerPosition;
    camera.target   = camera.position + gplayer.lookingDirection * 100.0f;

    BeginMode3D(camera);

    {  // Drawing world.
        struct {
            Vector3 pos;
            Vector3 size;
            Color   color;
        } cubes[] = {
            {{11, 3, 8}, {2, 6, 2}, GREEN},
            {{16, 3, 3}, {2, 6, 2}, BLUE},
            {{-15, 4, 11}, {2, 8, 2}, YELLOW},
            {{3, 2, -10}, {2, 4, 2}, RED},
            {{19, 1, 13}, {2, 2, 2}, MAGENTA},
            {{6, 3, 13}, {2, 6, 2}, MAGENTA},
            {{11, 6, 16}, {1, 12, 1}, GRAY},

            {{-25, 8, 11}, {2, 16, 2}, YELLOW},
            {{-25, 12, 16}, {2, 1, 8}, YELLOW},
            {{-25, 8, 21}, {2, 16, 2}, YELLOW},
        };

        int cubesCount = sizeof(cubes) / sizeof(cubes[0]);

        FOR_RANGE (int, i, cubesCount) {
            const auto  cube = cubes[i];
            const auto& pos  = cube.pos;
            const auto& size = cube.size;

            DrawCube(pos, size.x, size.y, size.z, cube.color);
            DrawCubeWires(pos, size.x, size.y, size.z, MAROON);
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
