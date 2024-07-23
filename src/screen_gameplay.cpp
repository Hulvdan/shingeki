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

    const float Speed       = 10.0f;  // m / s
    const float JumpImpulse = 80.0f;  // m
    const float Gravity     = -9.8f;  // m / s / s
    const float Mass        = 10.0f;  // kg

    const Vector3 RopesOffset = {0.2f, 0.0f, 0.0f};
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
    assert(gdata.states != nullptr);

    gplayer.currentState->OnExit();
    gplayer.currentState = gdata.states + (int)state;
    gplayer.currentState->OnEnter();
}

// Grounded functions
//----------------------------------------------------------------------------------
PlayerState_OnEnter_Function(Grounded_OnEnter) {}

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
                                * gplayer.Speed;

            gplayer.velocity += Vector3(dHoriz.x, 0, dHoriz.y);
        }
    }

    {  // Jumping.
        if (IsKeyPressed(KEY_SPACE)) {
            gplayer.velocity
                += ApplyImpulse(Vector3Up, gplayer.Mass, gplayer.JumpImpulse);

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

    {  // Player movement direction calculation.
        gplayer.velocity.x = 0;
        gplayer.velocity.z = 0;

        const Vector2 lookingHorizontalDirection
            = {gplayer.lookingDirection.x, gplayer.lookingDirection.z};

        const auto controlVector = GetPlayerMovementControlVector();

        if (controlVector.x != 0 || controlVector.y != 0) {
            const auto angle = atan2f(controlVector.y, controlVector.x);

            const auto dHoriz = Vector2Rotate(lookingHorizontalDirection, PI / 2 + angle)
                                * gplayer.Speed;

            gplayer.velocity += Vector3(dHoriz.x, 0, dHoriz.y);
        }
    }

    {  // Gravity.
        gplayer.velocity.y += dt * gplayer.Gravity;
    }

    {  // Movement.
        gplayer.position += gplayer.velocity * dt;
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

    // ------------------------------------------------------------

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

    gplayer.currentState->Update(dt);

    {
        const float    MaxDistance      = 20.0f;
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
                && (collision.distance < MaxDistance)  //
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
        if (gplayer.collided) {
            const auto from     = gplayer.position + gplayer.RopesOffset;
            const auto to       = gplayer.lookingAtCollision;
            const auto distance = Vector3Distance(to, from);

            const auto  middlePoint = (from + to) / 2.0f;
            const int   Slices      = 16;
            const float Radius      = 0.3f;

            Mesh  cylinderMesh = GenMeshCylinder(Radius, distance, Slices);
            Model model        = LoadModelFromMesh(cylinderMesh);
            assert(IsModelReady(model));

            const auto cross = Vector3(
                gplayer.lookingAtCollision.x - gplayer.position.x,
                0,
                gplayer.lookingAtCollision.z - gplayer.position.z
            );
            const auto cross2 = Vector3(
                gplayer.lookingAtCollision.x - gplayer.position.x,
                gplayer.lookingAtCollision.y - gplayer.position.y,
                gplayer.lookingAtCollision.z - gplayer.position.z
            );
            const auto axis = Vector3Normalize(Vector3CrossProduct(cross2, cross));

            DrawModelEx(
                model,
                from + axis * 1.0f,
                axis,
                (-Vector3Angle(cross2, cross) + PI / 2) * RAD2DEG,
                Vector3One(),
                YELLOW
            );
            UnloadModel(model);
        }

        if (gdata.gizmosEnabled && gplayer.collided)
            DrawLine3D(gplayer.position, gplayer.lookingAtCollision, WHITE);
    }
    EndMode3D();

    {  // Cross.
        const int  CrossSize  = 20;
        const auto CrossColor = WHITE;

        DrawLine(
            screenWidth / 2,
            (screenHeight - CrossSize) / 2,
            screenWidth / 2,
            (screenHeight + CrossSize) / 2,
            CrossColor
        );
        DrawLine(
            (screenWidth - CrossSize) / 2,
            screenHeight / 2,
            (screenWidth + CrossSize) / 2,
            screenHeight / 2,
            CrossColor
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
        "look %.2f %.2f %.2f",
        gplayer.lookingDirection.x,
        gplayer.lookingDirection.y,
        gplayer.lookingDirection.z
    ));
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
