/**********************************************************************************************
 *
 *   raylib - Advance Game template
 *
 *   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
 *
 *   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
 *
 *   This software is provided "as-is", without any express or implied warranty. In no
 *event will the authors be held liable for any damages arising from the use of this
 *software.
 *
 *   Permission is granted to anyone to use this software for any purpose, including
 *commercial applications, and to alter it and redistribute it freely, subject to the
 *following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not claim that
 *you wrote the original software. If you use this software in a product, an
 *acknowledgment in the product documentation would be appreciated but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not be
 *misrepresented as being the original software.
 *
 *     3. This notice may not be removed or altered from any source distribution.
 *
 **********************************************************************************************/

#define FOR_RANGE(type, variableName, maxValueExclusive) \
    for (type variableName = 0; (variableName) < (maxValueExclusive); (variableName)++)

//----------------------------------------------------------------------------------
// Module Constants Definition.
//----------------------------------------------------------------------------------

#define REVOLUTIONS2RAD (PI)
#define REVOLUTIONS2DEG ((PI) * (RAD2DEG))

static constexpr int fpsValues[] = {60, 20, 40};

//----------------------------------------------------------------------------------
// Module Variables Definition (local).
//----------------------------------------------------------------------------------
static int  currentFPSValueIndex  = 0;
static bool gizmosEnabled         = true;
static bool unzoomedCameraEnabled = true;

static struct GameDataType {
    int finishScreen = 0;

    Camera3D camera         = {};
    Vector3  playerPosition = {};
} gdata;

//----------------------------------------------------------------------------------
// Helper Functions Definition.
//----------------------------------------------------------------------------------

float MoveTowards(float v, float target, float maxDistance) {
    // assert(maxDistance >= 0);

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

Vector2 GetRectangleSize(Rectangle rect) {
    return {rect.width, rect.height};
}

//----------------------------------------------------------------------------------
// Gameplay Functions Definition.
//----------------------------------------------------------------------------------

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

    gdata.camera.position   = Vector3{0.0f, 10.0f, 10.0f};
    gdata.camera.target     = Vector3{0.0f, 0.0f, 0.0f};
    gdata.camera.up         = Vector3{0.0f, 1.0f, 0.0f};
    gdata.camera.fovy       = 45.0f;
    gdata.camera.projection = CAMERA_PERSPECTIVE;

    gdata.playerPosition = Vector3{0.0f, 0.0f, 0.0f};
}

// Gameplay Screen Update logic.
void UpdateGameplayScreen() {
    auto dt = GetFrameTime();

    // Press enter or tap to change to ENDING screen.
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
        gdata.finishScreen = 1;
        PlaySound(fxCoin);
    }

    {  // Controlling FPS.
        if (IsKeyPressed(KEY_F1)) {
            currentFPSValueIndex++;
            if (currentFPSValueIndex >= sizeof(fpsValues) / sizeof(currentFPSValueIndex))
                currentFPSValueIndex = 0;
            SetTargetFPS(fpsValues[currentFPSValueIndex]);
        }
    }

    {  // Enabling drawing gizmos.
        bool  move      = true;
        float direction = 0;

        if (IsKeyDown(KEY_A))
            direction = PI;
        else if (IsKeyDown(KEY_D))
            direction = 0;
        else if (IsKeyDown(KEY_W))
            direction = 1.0f * PI / 2.0f;
        else if (IsKeyDown(KEY_S))
            direction = 3.0f * PI / 2.0f;
        else
            move = false;

        if (move)
            gdata.playerPosition += Vector3RotateByAxisAngle(
                Vector3(dt, 0, 0), Vector3(0, 1, 0), direction
            );
    }

    {  // Enabling drawing gizmos.
        if (IsKeyPressed(KEY_F2))
            gizmosEnabled = !gizmosEnabled;
    }

    {  // Enabling unzoomed camera for debugging.
        if (IsKeyPressed(KEY_F3))
            unzoomedCameraEnabled = !unzoomedCameraEnabled;
    }
}

// Gameplay Screen Draw logic.
void DrawGameplayScreen() {
    DebugTextReset();

    const auto    screenWidth  = GetScreenWidth();
    const auto    screenHeight = GetScreenHeight();
    const Vector2 screenSize{(float)screenWidth, (float)screenHeight};

    DrawRectangle(0, 0, screenWidth, screenHeight, BLACK);

    auto& camera = gdata.camera;

    BeginMode3D(camera);
    {  // Drawing world.
        DrawCube(gdata.playerPosition + Vector3Up, 2.0f, 2.0f, 2.0f, GREEN);
        DrawCubeWires(gdata.playerPosition + Vector3Up, 2.0f, 2.0f, 2.0f, MAROON);
        DrawGrid(100, 1.0f);
    }
    if (gizmosEnabled) {
    }
    EndMode3D();

    DebugTextDraw(
        TextFormat("FPS: %i (press F1 to change)", fpsValues[currentFPSValueIndex])
    );
    DebugTextDraw(TextFormat("Toggle gizmos - F2"));
    DebugTextDraw(TextFormat("Toggle unzoomed camera - F3"));
}

// Gameplay Screen Unload logic.
void UnloadGameplayScreen() {}

// Gameplay Screen should finish?
int FinishGameplayScreen() {
    return gdata.finishScreen;
}
