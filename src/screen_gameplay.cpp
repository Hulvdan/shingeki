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

#include "raylib.h"
#include "raymath.h"
#include "global.h"
#include <cassert>

#define FOR_RANGE(type, variableName, maxValueExclusive) \
    for (type variableName = 0; (variableName) < (maxValueExclusive); (variableName)++)

//----------------------------------------------------------------------------------
// Module Constants Definition.
//----------------------------------------------------------------------------------

#define REVOLUTIONS2RAD (3.14159265358979f)
#define REVOLUTIONS2DEG (3.14159265358979f * (RAD2DEG))

static constexpr int fpsValues[] = {60, 20, 40};

//----------------------------------------------------------------------------------
// Module Variables Definition (local).
//----------------------------------------------------------------------------------
static int  currentFPSValueIndex  = 0;
static bool gizmosEnabled         = true;
static bool unzoomedCameraEnabled = true;

static int finishScreen = 0;

static struct GameDataType {
    Camera2D camera = {};

    Vector2 playerPos               = {};
    Vector2 playerSpeed             = {};
    float   playerMaxSpeed          = 800;
    float   playerSpeedAcceleration = 100.0f;
    float   playerSpeedDecay        = 5.0f;

    Vector2 friction             = {};
    float   frictionMinThreshold = 0;
    float   frictionMaxThreshold = 10;

    Vector2 playerAcceleration = {};

    float playerRotation             = 0;
    float playerRotationSpeed        = 0;
    float playerRotationAcceleration = 0.1f * REVOLUTIONS2RAD;
    float playerMaxRotationSpeed     = 0.3f * REVOLUTIONS2RAD;
    // should be < playerRotationSpeed
    float playerRotationDissolve = 0.05f * REVOLUTIONS2RAD;

    Vector2 playerDirection = {0};

    Texture2D shipTexture       = {};
    Texture2D planetTextures[3] = {};
    Texture2D mistTexture       = {};
} gdata;

static int lastDebugTextY = 0;

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

void BFLoadTexture(Texture2D* texture, const char* path) {
    auto image = LoadImage(path);
    *texture   = LoadTextureFromImage(image);
    UnloadImage(image);
}

void BFDrawTexture(
    const Texture2D* const tex,
    Vector2                pos,
    Vector2                anchor,
    float                  rotationDegrees,
    float                  scale,
    Color                  color
) {
    Rectangle sourceRec = {0.0f, 0.0f, (float)tex->width, (float)tex->height};
    Rectangle destRec
        = {pos.x, pos.y, (float)tex->width * scale, (float)tex->height * scale};

    DrawTexturePro(
        *tex,
        sourceRec,
        destRec,
        GetRectangleSize(destRec) * anchor,
        rotationDegrees,
        color
    );
}

void BFDrawDebugText(const char* text) {
    const auto padding = 4;
    const auto height  = 20;

    DrawText(text, padding, lastDebugTextY + padding, 20, RED);

    lastDebugTextY += height + padding;
}

//----------------------------------------------------------------------------------
// Gameplay Functions Definition.
//----------------------------------------------------------------------------------

Vector2 GetCameraFinalTarget() {
    auto result = gdata.playerPos + gdata.playerDirection * 150;
    return result;
}

void BFDrawMist(Vector2 pos, float scale) {
    BFDrawTexture(
        &gdata.mistTexture,
        pos,
        Vector2Zero(),
        0,  //
        scale,
        Fade(RED, 0.6f)
        // Fade(WHITE, 0.4f)
        // RED
        // WHITE
    );
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

    finishScreen = 0;

    gdata.camera.zoom   = 1;
    gdata.camera.target = GetCameraFinalTarget();

    BFLoadTexture(&gdata.shipTexture, "resources/ship.png");
    // BFLoadTexture(&gdata.mistTexture, "resources/dust.png");
    BFLoadTexture(&gdata.mistTexture, "resources/blue_nebula_01_1024x1024.png");
    BFLoadTexture(&gdata.planetTextures[0], "resources/planet_1.png");
    BFLoadTexture(&gdata.planetTextures[1], "resources/planet_2.png");
    BFLoadTexture(&gdata.planetTextures[2], "resources/planet_3.png");
}

// Gameplay Screen Update logic.
void UpdateGameplayScreen() {
    auto dt = GetFrameTime();

    // Press enter or tap to change to ENDING screen.
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
        finishScreen = 1;
        PlaySound(fxCoin);
    }

    {  // Player's rotation.
        float accelerationScale = 0;
        if (IsKeyDown(KEY_A))
            accelerationScale = -1;
        if (IsKeyDown(KEY_D))
            accelerationScale = 1;

        if (accelerationScale == 0)
            gdata.playerRotationSpeed = MoveTowards(
                gdata.playerRotationSpeed, 0, dt * gdata.playerRotationDissolve
            );
        else
            gdata.playerRotationSpeed = MoveTowards(
                gdata.playerRotationSpeed,
                accelerationScale * gdata.playerMaxRotationSpeed,
                dt * gdata.playerRotationAcceleration
            );

        gdata.playerRotation += dt * gdata.playerRotationSpeed;
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
        if (IsKeyPressed(KEY_F2))
            gizmosEnabled = !gizmosEnabled;
    }

    {  // Enabling unzoomed camera for debugging.
        if (IsKeyPressed(KEY_F3))
            unzoomedCameraEnabled = !unzoomedCameraEnabled;
    }

    {  // Enabling unzoomed camera for debugging.
        if (IsKeyPressed(KEY_F5)) {
            gdata.playerPos           = Vector2Zero();
            gdata.playerSpeed         = Vector2Zero();
            gdata.playerRotationSpeed = 0;
            gdata.camera.target       = GetCameraFinalTarget();
        }
    }

    {  // Player's position.
        gdata.playerDirection = Vector2Rotate(Vector2(1, 0), gdata.playerRotation);

        int movementScaleFactor = 0;
        if (IsKeyDown(KEY_W))
            movementScaleFactor = 1;
        if (IsKeyDown(KEY_S))
            movementScaleFactor = -1;

        // Decay
        gdata.playerSpeed = Vector2MoveTowards(
            gdata.playerSpeed, Vector2Zero(), gdata.playerSpeedDecay * dt
        );

        gdata.playerSpeed += (gdata.playerDirection * gdata.playerSpeedAcceleration)
                             * (dt * (float)movementScaleFactor);

        gdata.playerSpeed = Vector2Normalize(gdata.playerSpeed)
                            * Clamp(
                                Vector2Length(gdata.playerSpeed),
                                -gdata.playerMaxSpeed,
                                gdata.playerMaxSpeed
                            );

        gdata.playerPos += gdata.playerSpeed * dt;

        {  // Зацикливание мира. Идём долго в одну сторону -> возвращаемся с другой.
            const auto cycleScale = 3.0f;

            Vector2 off = {};

            if (gdata.playerPos.x > (float)gdata.mistTexture.width * cycleScale)
                off.x -= 2.0f * cycleScale * (float)gdata.mistTexture.width;
            if (gdata.playerPos.x < -(float)gdata.mistTexture.width * cycleScale)
                off.x += 2.0f * cycleScale * (float)gdata.mistTexture.width;
            if (gdata.playerPos.y > (float)gdata.mistTexture.height * cycleScale)
                off.y -= 2.0f * cycleScale * (float)gdata.mistTexture.height;
            if (gdata.playerPos.y < -(float)gdata.mistTexture.height * cycleScale)
                off.y += 2.0f * cycleScale * (float)gdata.mistTexture.height;

            gdata.playerPos += off;
            gdata.camera.target += off;
        }
    }

    gdata.camera.target
        = Vector2ExponentialDecay(gdata.camera.target, GetCameraFinalTarget(), 2, dt);
}

// Gameplay Screen Draw logic.
void DrawGameplayScreen() {
    lastDebugTextY = 0;

    const auto    screenWidth  = GetScreenWidth();
    const auto    screenHeight = GetScreenHeight();
    const Vector2 screenSize{(float)screenWidth, (float)screenHeight};

    DrawRectangle(0, 0, screenWidth, screenHeight, BLACK);

    auto& camera  = gdata.camera;
    camera.offset = screenSize / 2.0f - gdata.playerSpeed;

    // World Rendering.
    //---------------------------------------------------------

    // Получение координат мира по краям экрана.
    gdata.camera.zoom = 2;
    const auto p0     = GetScreenToWorld2D(Vector2Zero(), camera);
    const auto p1     = GetScreenToWorld2D(screenSize, camera);

    // Отадление камеры, чтобы видно было область за пределами той, что видит игрок.
    if (unzoomedCameraEnabled)
        gdata.camera.zoom = 1.6f;

    BeginMode2D(camera);
    {  // Drawing background.
        const float backgroundScale = 2.0f;
        const int   safetyPadding   = 16;

        const int sizeX = lrintf((float)gdata.mistTexture.width * backgroundScale);
        const int sizeY = lrintf((float)gdata.mistTexture.height * backgroundScale);

        const int timesX
            = CeilDivision(int(p1.x - p0.x + 2 * safetyPadding) + 1, sizeX) + 1;
        const int timesY
            = CeilDivision(int(p1.y - p0.y + 2 * safetyPadding) + 1, sizeY) + 1;

        const int startingX = Floor(int(p0.x), sizeX) - safetyPadding;
        const int startingY = Floor(int(p0.y), sizeY) - safetyPadding;

        FOR_RANGE (int, dy, timesY) {
            const float y = (float)startingY
                            + (float)(dy * gdata.mistTexture.height) * backgroundScale;
            FOR_RANGE (int, dx, timesX) {
                const float x = (float)startingX
                                + (float)(dx * gdata.mistTexture.width) * backgroundScale;
                BFDrawMist(Vector2(x, y), backgroundScale);
            }
        }
    }
    {  // Drawing planets.
        const Vector2 planetPositions[] = {
            {-100, 0},
            {50, 300},
            {200, -100},
        };

        FOR_RANGE (int, i, 3) {
            auto pos = planetPositions[i] * 2;
            BFDrawTexture(gdata.planetTextures + i, pos, Vector2Zero(), 0, 2, WHITE);
        }
    }
    {  // Drawing player.
        BFDrawTexture(
            &gdata.shipTexture,
            gdata.playerPos,
            Vector2(0.2f, 0.5f),
            gdata.playerRotation * RAD2DEG,
            1,
            WHITE
        );
    }

    if (gizmosEnabled) {
        {  // World's debug stuff.
            auto cameraFinalTarget = GetCameraFinalTarget();
            DrawLine(
                int(gdata.playerPos.x),
                int(gdata.playerPos.y),
                int(camera.target.x),
                int(camera.target.y),
                GREEN
            );
            DrawLine(
                int(gdata.playerPos.x),
                int(gdata.playerPos.y),
                int(cameraFinalTarget.x),
                int(cameraFinalTarget.y),
                GREEN
            );
            DrawLine(
                int(gdata.playerPos.x),
                int(gdata.playerPos.y),
                int(gdata.playerPos.x + gdata.playerSpeed.x),
                int(gdata.playerPos.y + gdata.playerSpeed.y),
                RED
            );
        }
        {  // Drawing Screen Bounds when camera zoom is 1
            DrawLine(int(p0.x) - 1, int(p0.y) - 1, int(p1.x) + 1, int(p0.y) - 1, RED);
            DrawLine(int(p0.x) - 1, int(p0.y) - 1, int(p0.x) - 1, int(p1.y) + 1, RED);
            DrawLine(int(p1.x) + 1, int(p1.y) + 1, int(p1.x) + 1, int(p0.y) - 1, RED);
            DrawLine(int(p1.x) + 1, int(p1.y) + 1, int(p0.x) - 1, int(p1.y) + 1, RED);
        }
    }
    EndMode2D();
    //--------------------------------------------------------------------------------------

    gdata.camera.zoom = 0.3f;

    BFDrawDebugText(
        TextFormat("FPS: %i (press F1 to change)", fpsValues[currentFPSValueIndex])
    );
    BFDrawDebugText(TextFormat("Toggle gizmos - F2"));
    BFDrawDebugText(TextFormat("Toggle unzoomed camera - F3"));
    BFDrawDebugText(TextFormat("Reset location - F5"));
    BFDrawDebugText(TextFormat("Toggle Fullscreen - F11"));
    BFDrawDebugText(
        TextFormat("pos: %03i %03i", (int)gdata.playerPos.x, (int)gdata.playerPos.y)
    );
    BFDrawDebugText(TextFormat(
        "camera.target: %03i %03i", (int)gdata.camera.target.x, (int)gdata.camera.target.y
    ));
    BFDrawDebugText(
        TextFormat("player speed: %03i", (int)Vector2Length(gdata.playerSpeed))
    );
    BFDrawDebugText(TextFormat("player rot speed: %02.2f", gdata.playerRotationSpeed));
}

// Gameplay Screen Unload logic.
void UnloadGameplayScreen() {
    UnloadTexture(gdata.shipTexture);
    UnloadTexture(gdata.mistTexture);
    UnloadTexture(gdata.planetTextures[0]);
    UnloadTexture(gdata.planetTextures[1]);
    UnloadTexture(gdata.planetTextures[2]);
}

// Gameplay Screen should finish?
int FinishGameplayScreen() {
    return finishScreen;
}
