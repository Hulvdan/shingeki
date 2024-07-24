static constexpr int fpsValues[] = {60, 20, 40};

struct PlayerState;

globalVar struct {
    int  currentFPSValueIndex = 0;
    bool gizmosEnabled        = true;

    int finishScreen = 0;

    Camera3D camera = {};

    PlayerState* states = nullptr;

    Sound  fxJump;
    int    fxFootstepsCount = 0;
    Sound* fxFootsteps      = nullptr;

    struct {
        Vector3 pos;
        Vector3 size;
        Color   color;
    } cubes[10] = {
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

    std::vector<Vector3> linesToDraw;
} gdata;

globalVar struct {
    float   rotationY          = 0;
    float   rotationHorizontal = 0;
    Vector3 lookingDirection   = {};

    Vector3 position = {0.0f, 0.0f, 10.0f};
    Vector3 velocity = {};

    PlayerState* currentState = nullptr;

    bool    collided           = false;
    Vector3 lookingAtCollision = {};

    bool    ropeActivated = false;
    float   ropeLength    = 0;
    Vector3 ropePos       = {};

    const float speed       = 10.0f;  // m / s
    const float jumpImpulse = 80.0f;  // m
    const float gravity     = -9.8f;  // m / s / s
    const float mass        = 10.0f;  // kg
} gplayer;

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

Vector3 ApplyImpulse(Vector3 direction, float mass, float forceValue) {
    return direction * (forceValue / mass);
}

Vector3 HorizontalAxisOf(Vector3 value) {
    return Vector3Normalize(Vector3CrossProduct(value, Vector3Up));
}

Vector3 DisplaceToTheSide(Vector3 value, float displacement) {
    return value + HorizontalAxisOf(value) * displacement;
}

void DrawRope(Vector3 from, Vector3 to) {
    const auto distance = Vector3Distance(to, from);

    const auto  middlePoint = (from + to) / 2.0f;
    const int   slices      = 16;
    const float radius      = 0.1f;

    Mesh  cylinderMesh = GenMeshCylinder(radius, distance, slices);
    Model model        = LoadModelFromMesh(cylinderMesh);
    Assert(IsModelReady(model));

    const auto axis  = HorizontalAxisOf(to - from);
    const auto angle = -Vector3Angle(to - from, Vector3Up);

    DrawModelEx(model, from, axis, angle * RAD2DEG, Vector3One(), YELLOW);
    if (gdata.gizmosEnabled)
        DrawModelWiresEx(model, from, axis, angle * RAD2DEG, Vector3One(), MAROON);

    UnloadModel(model);
}

//----------------------------------------------------------------------------------
// Player State Machine.
//----------------------------------------------------------------------------------

enum class PlayerStates {
    GROUNDED = 0,  // good movement control
    AIRBORNE,      // air control, movement is not instant, can't jump again
};

#define PlayerState_OnEnter_Function(name_) void name_()
#define PlayerState_OnExit_Function(name_) void name_()
#define PlayerState_Update_Function(name_) void name_(float dt)

struct PlayerState {
    PlayerState_OnEnter_Function((*OnEnter));
    PlayerState_OnExit_Function((*OnExit));
    PlayerState_Update_Function((*Update));
};

void SwitchState(PlayerStates state) {
    Assert(gdata.states != nullptr);

    gplayer.currentState->OnExit();
    gplayer.currentState = gdata.states + (int)state;
    gplayer.currentState->OnEnter();
}

// Grounded functions
//----------------------------------------------------------------------------------
PlayerState_OnEnter_Function(Grounded_OnEnter) {
    gplayer.ropeActivated = false;
}

PlayerState_OnExit_Function(Grounded_OnExit) {}

PlayerState_Update_Function(Grounded_Update) {
    {  // Player camera rotation.
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

    {  // Player movement direction calculation.
        gplayer.velocity.x = 0;
        gplayer.velocity.z = 0;

        const Vector2 lookingHorizontalDirection
            = {gplayer.lookingDirection.x, gplayer.lookingDirection.z};

        const auto controlVector = GetPlayerMovementControlVector();

        if (controlVector.x != 0 || controlVector.y != 0) {
            const auto angle = atan2f(controlVector.y, controlVector.x);

            const auto dHoriz = Vector2Rotate(lookingHorizontalDirection, PI / 2 + angle)
                                * gplayer.speed;

            gplayer.velocity += Vector3(dHoriz.x, 0, dHoriz.y);
        }
    }

    {  // Jumping.
        if (IsKeyPressed(KEY_SPACE)) {
            gplayer.velocity
                += ApplyImpulse(Vector3Up, gplayer.mass, gplayer.jumpImpulse);

            SwitchState(PlayerStates::AIRBORNE);
            PlaySound(gdata.fxJump);
        }
    }

    {  // Movement.
        gplayer.position += gplayer.velocity * dt;
    }
}

// Airborne functions
//----------------------------------------------------------------------------------
PlayerState_OnEnter_Function(Airborne_OnEnter) {}

PlayerState_OnExit_Function(Airborne_OnExit) {}

int signof(float value) {
    if (value > 0)
        return 1;
    if (value < 0)
        return -1;
    return 0;
}

Vector3 TransformVelocityBasedOnRopeDirection(Vector3 velocity, Vector3 ropeDirection) {
    const auto axis = Vector3CrossProduct(Vector3Up, ropeDirection);

#if 1
    auto perp = Vector3Perpendicular(ropeDirection);
#else
    auto    d    = ropeDirection;
    Vector3 v1   = {0, 1, -d.y / d.z};
    Vector3 v2   = {1, -d.x / d.y, 0};
    auto    perp = Vector3CrossProduct(v2, v1);
#endif

    if (gdata.gizmosEnabled) {
        const float   displacement = 0.3f;
        const Vector3 points[]     = {
            {displacement, 0, displacement},
            {-displacement, 0, displacement},
            {-displacement, 0, -displacement},
            {displacement, 0, -displacement},
        };
        const int pointIndices[] = {0, 1, 1, 2, 2, 3, 3, 0, 1, 3, 0, 2};

        FOR_RANGE (int, i, 6) {
            auto p1 = points[pointIndices[i * 2]];
            auto p2 = points[pointIndices[i * 2 + 1]];

            auto axis      = HorizontalAxisOf(ropeDirection);
            auto angle     = -Vector3Angle(Vector3Up, ropeDirection);
            auto p1Rotated = Vector3RotateByAxisAngle(p1, axis, angle);
            auto p2Rotated = Vector3RotateByAxisAngle(p2, axis, angle);

            gdata.linesToDraw.push_back(gplayer.position + p1Rotated);
            gdata.linesToDraw.push_back(gplayer.position + p2Rotated);
        }
        gdata.linesToDraw.push_back(gplayer.position);
        gdata.linesToDraw.push_back(gplayer.ropePos);
    }

    auto angle = Vector3Angle(perp, velocity);
    angle *= -signof(Vector3DotProduct(Vector3Up, ropeDirection));

    // angle *= signof(Vector3DotProduct(axis, perp));
    // angle *= -signof(Vector3DotProduct(velocity, ropeDirection));
    // angle *= signof(Vector3DotProduct(velocity, axis));

    // angle *= signof(Vector3DotProduct(perp, ropeDirection));
    // angle *= signof(Vector3DotProduct(velocity, perp));
    // angle *= -signof(Vector3DotProduct(axis, perp));

    auto result = Vector3RotateByAxisAngle(velocity, axis, angle);
    if (velocity.y > 0) {
        if ((result.x) > 0 && (result.z < 0))
            result.y *= -1;

        if ((result.x) < 0 && (result.z < 0))
            result.y *= -1;
    }
    else {
        if ((result.x) > 0 && (result.z > 0))
            result.y *= -1;

        if ((result.x) < 0 && (result.z > 0))
            result.y *= -1;
    }
    // if (ropeDirection.y < 0) {
    //     result.x *= -1;
    //     result.z *= -1;
    // }
    // else {
    //     // result.y *= -1;
    // }
    // if (ropeDirection.y > 0) {
    //     result.x *= -1;
    //     result.z *= -1;
    //     result.y *= -1;
    // }
    // else {
    //     result *= -1;
    // }
    return result;
}

TEST_CASE ("TransformVelocityBasedOnRopeDirection") {
    float velocities[] = {-1, 1};
    for (float yvelocity : velocities) {
        SUBCASE(
            TextFormat("y velocity is %f, y rope direction is %f", yvelocity, -yvelocity)
        ) {
            const Vector3 vel = {0, yvelocity, 0};

            auto tPosXPosZ
                = TransformVelocityBasedOnRopeDirection(vel, {1, -yvelocity, 1});
            auto tPosXNegZ
                = TransformVelocityBasedOnRopeDirection(vel, {1, -yvelocity, -1});
            auto tNegXPosZ
                = TransformVelocityBasedOnRopeDirection(vel, {-1, -yvelocity, 1});
            auto tNegXNegZ
                = TransformVelocityBasedOnRopeDirection(vel, {-1, -yvelocity, -1});

            Assert(tPosXPosZ.x > 0);
            Assert(tPosXPosZ.z > 0);

            Assert(tPosXNegZ.x > 0);
            Assert(tPosXNegZ.z < 0);

            Assert(tNegXPosZ.x < 0);
            Assert(tNegXPosZ.z > 0);

            Assert(tNegXNegZ.x < 0);
            Assert(tNegXNegZ.z < 0);

            // auto tPosX = TransformVelocityBasedOnRopeDirection(vel, {1, -yvelocity,
            // 0}); auto tNegX = TransformVelocityBasedOnRopeDirection(vel, {-1,
            // -yvelocity, 0}); auto tPosZ = TransformVelocityBasedOnRopeDirection(vel,
            // {0, -yvelocity, 1}); auto tNegZ =
            // TransformVelocityBasedOnRopeDirection(vel, {0, -yvelocity, -1});
            //
            // Assert(tPosX.x > 0);
            // Assert(FloatEquals(tPosX.z, 0));
            //
            // Assert(tNegX.x < 0);
            // Assert(FloatEquals(tNegX.z, 0));
            //
            // Assert(tPosZ.z > 0);
            // Assert(FloatEquals(tPosZ.x, 0));
            //
            // Assert(tNegZ.z < 0);
            // Assert(FloatEquals(tNegZ.x, 0));
            //
            Assert(yvelocity * tPosXPosZ.y > 0);
            Assert(yvelocity * tPosXNegZ.y > 0);
            Assert(yvelocity * tNegXPosZ.y > 0);
            Assert(yvelocity * tNegXNegZ.y > 0);
            // Assert(yvelocity * tPosX.y > 0);
            // Assert(yvelocity * tNegX.y > 0);
            // Assert(yvelocity * tPosZ.y > 0);
            // Assert(yvelocity * tNegZ.y > 0);
        }
    }
}

PlayerState_Update_Function(Airborne_Update) {
    {  // Player camera rotation.
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

    // {  // Player movement direction calculation.
    //     gplayer.velocity.x = 0;
    //     gplayer.velocity.z = 0;
    //
    //     const Vector2 lookingHorizontalDirection
    //         = {gplayer.lookingDirection.x, gplayer.lookingDirection.z};
    //
    //     const auto controlVector = GetPlayerMovementControlVector();
    //
    //     if (controlVector.x != 0 || controlVector.y != 0) {
    //         const auto angle = atan2f(controlVector.y, controlVector.x);
    //
    //         const auto dHoriz = Vector2Rotate(lookingHorizontalDirection, PI / 2 +
    //         angle)
    //                             * gplayer.speed;
    //
    //         gplayer.velocity += Vector3(dHoriz.x, 0, dHoriz.y);
    //     }
    // }

    if (IsMouseButtonPressed(0)) {
        if (gplayer.ropeActivated) {
            gplayer.ropeActivated = false;
        }
        else if (gplayer.collided) {
            gplayer.ropeActivated = true;
            gplayer.ropePos       = gplayer.lookingAtCollision;
            gplayer.ropeLength    = Vector3Distance(gplayer.ropePos, gplayer.position);
        }
    }

    // if (gplayer.ropeActivated) {
    //     if (!IsMouseButtonDown(0)) {
    //         gplayer.ropeActivated = false;
    //     }
    // }

    {  // gravity.
        gplayer.velocity.y += dt * gplayer.gravity;
    }

    {  // Movement.
        auto& position = gplayer.position;

        auto oldPos = position;
        position += gplayer.velocity * dt;

        if (gplayer.ropeActivated) {
            auto& ropePos = gplayer.ropePos;

            // float oldDist = Vector3Distance(oldPos, ropePos);
            float newDist = Vector3Distance(position, ropePos);

            // bool  oldDistanceIsSufficient = oldDist <= gplayer.ropeLength;
            bool newDistanceIsSufficient = newDist <= gplayer.ropeLength;
            if (!newDistanceIsSufficient) {
                position
                    = ropePos + Vector3Normalize(position - ropePos) * gplayer.ropeLength;

                gplayer.velocity = TransformVelocityBasedOnRopeDirection(
                    gplayer.velocity, ropePos - position
                );
            }
        }
    }

    {
        if (gplayer.position.y < 0) {
            gplayer.position.y = 0;

            if (gplayer.velocity.y < 0)
                gplayer.velocity.y = 0;

            SwitchState(PlayerStates::GROUNDED);
        }
    }
}

//----------------------------------------------------------------------------------
// Helper Functions Definition.
//----------------------------------------------------------------------------------
Vector2 GetRectangleSize(Rectangle rect) {
    return {rect.width, rect.height};
}

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition.
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic.
void InitGameplayScreen(Arena& arena) {
    // Global Variables Initialization
    // ------------------------------------------------------------
    if (gdata.states == nullptr) {
        auto& states = gdata.states;
        states       = AllocateArray(arena, PlayerState, 2);

        states[(int)PlayerStates::GROUNDED]
            = {Grounded_OnEnter, Grounded_OnExit, Grounded_Update};
        states[(int)PlayerStates::AIRBORNE]
            = {Airborne_OnEnter, Airborne_OnExit, Airborne_Update};
    }
    if (gdata.fxFootsteps == nullptr) {
        auto& footsteps   = gdata.fxFootsteps;
        gdata.fxFootsteps = AllocateArray(arena, Sound, 5);

        FOR_RANGE (int, i, 5) {
            gdata.fxFootsteps[i] = LoadSound(
                TextFormat("resources/screens/gameplay/footstep_%i.wav.wav", i)
            );
        }
    }
    gdata.fxJump = LoadSound("resources/screens/gameplay/jump.wav");

    if (gplayer.currentState == nullptr)
        gplayer.currentState = gdata.states + (int)PlayerStates::GROUNDED;

    gdata.finishScreen = 0;

    gdata.camera.target     = Vector3{0.0f, 0.0f, 0.0f};
    gdata.camera.up         = Vector3{0.0f, 1.0f, 0.0f};
    gdata.camera.fovy       = 55.0f;
    gdata.camera.projection = CAMERA_PERSPECTIVE;
    // ------------------------------------------------------------

    DisableCursor();
}

// Gameplay Screen Update logic.
void UpdateGameplayScreen() {
    auto dt = GetFrameTime();

    // Press enter or tap to change to ENDING screen.
    // if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
    if (IsKeyPressed(KEY_ENTER))
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

    {  // Enabling drawing gizmos.
        if (IsKeyPressed(KEY_F2))
            gdata.gizmosEnabled = !gdata.gizmosEnabled;
    }

    {  // Removing temporary debug lines.
        if (IsKeyPressed(KEY_F3))
            gdata.linesToDraw.clear();
    }

    gplayer.currentState->Update(dt);

    {
        const float    maxDistance      = 20.0f;
        const Vector3& lookingDirection = gplayer.lookingDirection;
        const Vector3& playerPosition   = gplayer.position;

        bool    collided             = false;
        float   minCollisionDistance = floatInf;
        Vector3 collisionPoint       = {};
        // Vector3 collisionNormal      = {};

        Ray ray = {gplayer.position + Vector3Up * 2.0f, gplayer.lookingDirection};

        int cubesCount = sizeof(gdata.cubes) / sizeof(gdata.cubes[0]);
        FOR_RANGE (int, i, cubesCount) {
            const auto cube = gdata.cubes[i];

            BoundingBox box = {cube.pos - cube.size / 2.0f, cube.pos + cube.size / 2.0f};

            RayCollision collision = GetRayCollisionBox(ray, box);

            if (collision.hit                          //
                && (collision.distance < maxDistance)  //
                && (minCollisionDistance > collision.distance))
            {
                minCollisionDistance = collision.distance;

                collisionPoint = collision.point;
                // collisionNormal = collision.normal;

                collided = true;
            }
        }

        if (collided) {
            gplayer.collided           = true;
            gplayer.lookingAtCollision = collisionPoint;
        }
        else
            gplayer.collided = false;
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
    camera.position = gplayer.position + Vector3Up * 2.0f;
    camera.target   = camera.position + gplayer.lookingDirection * 100.0f;

    BeginMode3D(camera);
    {  // Drawing world.
        int cubesCount = sizeof(gdata.cubes) / sizeof(gdata.cubes[0]);

        FOR_RANGE (int, i, cubesCount) {
            const auto  cube = gdata.cubes[i];
            const auto& pos  = cube.pos;
            const auto& size = cube.size;

            DrawCubeV(pos, size, cube.color);
            DrawCubeWiresV(pos, size, MAROON);
        }

        DrawGrid(100, 1.0f);
    }
    {  // Drawing ropes.
        if (gplayer.ropeActivated) {
            Vector3 from = gplayer.position;
            // Смещаем в сторону.
            from += HorizontalAxisOf(gplayer.lookingDirection) * 2.0f;
            // Смещаем вниз.
            from -= Vector3Up * 0.5f;

            DrawRope(from, gplayer.ropePos);
        }
    }
    {  // Drawing lines 3D.
        FOR_RANGE (int, i, gdata.linesToDraw.size() / 2) {
            auto& p1 = gdata.linesToDraw[i * 2];
            auto& p2 = gdata.linesToDraw[i * 2 + 1];
            DrawLine3D(p1, p2, WHITE);
        }
        // gdata.linesToDraw.clear();
    }
    EndMode3D();

    {  // Cross.
        const int size = 20;

        auto color = WHITE;
        if (gplayer.collided)
            color = GREEN;
        if (gplayer.ropeActivated)
            color = RED;

        DrawLine(
            screenWidth / 2,
            (screenHeight - size) / 2,
            screenWidth / 2,
            (screenHeight + size) / 2,
            color
        );
        DrawLine(
            (screenWidth - size) / 2,
            screenHeight / 2,
            (screenWidth + size) / 2,
            screenHeight / 2,
            color
        );
    }

    DebugTextDraw(
        TextFormat("FPS: %i (press F1 to change)", fpsValues[gdata.currentFPSValueIndex])
    );
    DebugTextDraw("Toggle gizmos - F2");
    DebugTextDraw(TextFormat(
        "pos %.2f %.2f %.2f", gplayer.position.x, gplayer.position.y, gplayer.position.z
    ));
    DebugTextDraw(TextFormat(
        "vel %.2f %.2f %.2f", gplayer.velocity.x, gplayer.velocity.y, gplayer.velocity.z
    ));
    DebugTextDraw(TextFormat(
        "look %.2f %.2f %.2f",
        gplayer.lookingDirection.x,
        gplayer.lookingDirection.y,
        gplayer.lookingDirection.z
    ));
    DebugTextDraw(TextFormat("rope length %.2f", gplayer.ropeLength));
}

// Gameplay Screen Unload logic.
void UnloadGameplayScreen() {
    EnableCursor();

    if (gdata.fxFootsteps != nullptr) {
        FOR_RANGE (int, i, 5) {
            UnloadSound(gdata.fxFootsteps[i]);
        }
    }
    UnloadSound(gdata.fxJump);
}

// Gameplay Screen should finish?
int FinishGameplayScreen() {
    return gdata.finishScreen;
}
