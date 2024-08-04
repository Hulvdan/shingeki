static constexpr int fpsValues[] = {60, 20, 40};
#include <sstream>

struct PlayerState;

const int PARTICLES_PER_SHADER_INSTANCE = 1024;
const int NUMBER_OF_INSTANCES           = 1;
const int NUM_PARTICLES = PARTICLES_PER_SHADER_INSTANCE * NUMBER_OF_INSTANCES;

float GetRandomFloat(float from, float to) {
    return from + (to - from) * (float)GetRandomValue(0, INT_MAX) / INT_MAX;
}

struct Vector3Int {
    int x;
    int y;
    int z;
};

struct CubeVoxel {
    Vector3Int pos;
    int        colorIndex;
};

globalVar struct {
    int  currentFPSValueIndex;
    bool gizmosEnabled = true;

    int finishScreen;

    Camera3D camera;

    PlayerState* states;

    Sound  fxJump;
    Sound  fxBoost;
    Sound  fxDash;
    Sound  fxGrapple;
    Sound  fxGrappleBack;
    int    fxFootstepsCount;
    Sound* fxFootsteps;

    std::vector<CubeVoxel> cubes;
    std::vector<Color>     colors;

    std::vector<Vector3> linesToDraw;
    std::vector<Color>   colorsOfLines;

    // Particles.
    // ref: https://github.com/arceryz/raylib-gpu-particles/blob/master/main.c
    Shader   particleShader;
    int      ssbo0;
    int      ssbo1;
    int      ssbo2;
    Vector4* positions;
    Vector4* velocities;
    float*   timesOfCreation;
    int      nextToGenerateParticleIndex;
    int      particleVao;
} gdata;

globalVar struct {
    float   rotationY;
    float   rotationHorizontal;
    Vector3 lookingDirection;

    Vector3 position = {0.0f, 0.0f, 1.0f};
    Vector3 velocity;

    PlayerState* currentState;

    bool    collided;
    Vector3 lookingAtCollision;

    bool    ropeActivated;
    float   ropeLength;
    Vector3 ropePos;

    double buttonGrapplePressedTime    = -floatInf;
    double buttonJumpPressedTime       = -floatInf;
    double buttonBoostPressedTime      = -floatInf;
    double buttonDashPressedTime       = -floatInf;
    double buttonClearPathsPressedTime = -floatInf;

    double lastBoostTime = -floatInf;
    double lastDashTime  = -floatInf;

    inline static const float fromDefaultToDashFovDuration = 0.1f;
    inline static const float fromDashToDefaultFovDuration = 1.0f;
    inline static const float defaultFov                   = 55.0f;
    inline static const float dashedFov                    = 75.0f;

    inline static const float boostSoundInterval = 0.13f;

    inline static const float airSpeed      = 2.0f;
    inline static const float speed         = 10.0f;   // m / s
    inline static const float jumpImpulse   = 80.0f;   // m
    inline static const float gravity       = -10.0f;  // m / s / s
    inline static const float velocityDecay = 0.1f;
    inline static const float mass          = 10.0f;  // kg

    inline static const float boostAmount = 3.3f;
    inline static const float maxVelocity = 28.0f;
    inline static const float dashImpulse = 200.0f;

    inline static const float particlesAmountPerSecond = 300.0f;
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
    const Color color    = {211, 202, 181, 255};
    const auto  distance = Vector3Distance(to, from);

    const auto  middlePoint = (from + to) / 2.0f;
    const int   slices      = 16;
    const float radius      = 0.1f;

    Mesh  cylinderMesh = GenMeshCylinder(radius, distance, slices);
    Model model        = LoadModelFromMesh(cylinderMesh);
    Assert(IsModelReady(model));

    const auto axis  = HorizontalAxisOf(to - from);
    const auto angle = -Vector3Angle(to - from, Vector3Up);

    DrawModelEx(model, from, axis, angle * RAD2DEG, Vector3One(), color);
    if (gdata.gizmosEnabled)
        DrawModelWiresEx(model, from, axis, angle * RAD2DEG, Vector3One(), BLACK);

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
            gplayer.buttonJumpPressedTime = GetTime();
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
    if (gdata.gizmosEnabled) {
        const float   displacement = 0.3f;
        const Vector3 points[]     = {
            {displacement, 0, displacement},
            {-displacement, 0, displacement},
            {-displacement, 0, -displacement},
            {displacement, 0, -displacement},
        };
        const int pointIndices[] = {0, 1, 1, 2, 2, 3, 3, 0, 1, 3, 0, 2};

        // FOR_RANGE (int, i, 6) {
        //     auto p1 = points[pointIndices[i * 2]];
        //     auto p2 = points[pointIndices[i * 2 + 1]];
        //
        //     auto axis      = HorizontalAxisOf(ropeDirection);
        //     auto angle     = -Vector3Angle(Vector3Up, ropeDirection);
        //     auto p1Rotated = Vector3RotateByAxisAngle(p1, axis, angle);
        //     auto p2Rotated = Vector3RotateByAxisAngle(p2, axis, angle);
        //
        //     if (gdata.gizmosEnabled) {
        //         gdata.linesToDraw.push_back(gplayer.position + p1Rotated);
        //         gdata.linesToDraw.push_back(gplayer.position + p2Rotated);
        //         gdata.colorsOfLines.push_back(WHITE);
        //     }
        // }
        if (gdata.gizmosEnabled) {
            gdata.linesToDraw.push_back(gplayer.position);
            gdata.linesToDraw.push_back(
                gplayer.position
                + Vector3Normalize(gplayer.ropePos - gplayer.position) * 0.2f
            );
            gdata.colorsOfLines.push_back(WHITE);

            gdata.linesToDraw.push_back(gplayer.ropePos);
            gdata.linesToDraw.push_back(
                gplayer.ropePos
                + Vector3Normalize(gplayer.position - gplayer.ropePos) * 0.2f
            );
            gdata.colorsOfLines.push_back(WHITE);
        }
    }

    auto axis     = HorizontalAxisOf(ropeDirection);
    auto angle    = -Vector3Angle(Vector3Up, ropeDirection);
    auto pRotated = Vector3Normalize(
        Vector3RotateByAxisAngle({ropeDirection.x, 0, ropeDirection.z}, axis, angle)
    );

    auto result = pRotated * Vector3DotProduct(pRotated, velocity)
                  + axis * Vector3DotProduct(axis, velocity);

    if (gdata.gizmosEnabled) {
        // gdata.linesToDraw.push_back(gplayer.position + pRotated);
        // gdata.linesToDraw.push_back(gplayer.position);
        // gdata.colorsOfLines.push_back(RED);

        gdata.linesToDraw.push_back(gplayer.position + Vector3Normalize(result));
        gdata.linesToDraw.push_back(gplayer.position);
        gdata.colorsOfLines.push_back(GREEN);
    }

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

            auto tPosX = TransformVelocityBasedOnRopeDirection(vel, {1, -yvelocity, 0});
            auto tNegX = TransformVelocityBasedOnRopeDirection(vel, {-1, -yvelocity, 0});
            auto tPosZ = TransformVelocityBasedOnRopeDirection(vel, {0, -yvelocity, 1});
            auto tNegZ = TransformVelocityBasedOnRopeDirection(vel, {0, -yvelocity, -1});

            Assert(tPosX.x > 0);
            Assert(FloatEquals(tPosX.z, 0));

            Assert(tNegX.x < 0);
            Assert(FloatEquals(tNegX.z, 0));

            Assert(tPosZ.z > 0);
            Assert(FloatEquals(tPosZ.x, 0));

            Assert(tNegZ.z < 0);
            Assert(FloatEquals(tNegZ.x, 0));

            Assert(yvelocity * tPosXPosZ.y > 0);
            Assert(yvelocity * tPosXNegZ.y > 0);
            Assert(yvelocity * tNegXPosZ.y > 0);
            Assert(yvelocity * tNegXNegZ.y > 0);
            Assert(yvelocity * tPosX.y > 0);
            Assert(yvelocity * tNegX.y > 0);
            Assert(yvelocity * tPosZ.y > 0);
            Assert(yvelocity * tNegZ.y > 0);
        }
    }
}

void UpdateSSBOAsRingBuffer(
    int   ssboID,
    void* data,
    int   dataElementSize,
    int   startedIndex,
    int   updatedElementsCount,
    int   ssboElementsCount
) {
#if 1
    rlUpdateShaderBuffer(ssboID, data, dataElementSize * ssboElementsCount, 0);
#else
    int finishedIndex = (startedIndex + updatedElementsCount) % ssboElementsCount;

    if (startedIndex < finishedIndex) {
        int   offset     = dataElementSize * startedIndex;
        void* dataOffset = (void*)((char*)data + offset);

        rlUpdateShaderBuffer(
            ssboID, dataOffset, dataElementSize * updatedElementsCount, offset
        );
    }
    else {
        int firstUpdate  = ssboElementsCount - startedIndex;
        int secondUpdate = updatedElementsCount - firstUpdate;

        void* firstDataOffset = (void*)((char*)data + firstUpdate);

        rlUpdateShaderBuffer(
            ssboID,
            firstDataOffset,
            dataElementSize * firstUpdate,
            dataElementSize * startedIndex
        );
        rlUpdateShaderBuffer(ssboID, data, dataElementSize * secondUpdate, 0);
    }
#endif
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

    {  // Player movement direction calculation.
        auto controlVector = GetPlayerMovementControlVector();
        controlVector.y *= -1;

        auto axis = HorizontalAxisOf(gplayer.lookingDirection);

        // TODO: Возможно, стоит ограничивать только ограничения по
        // направлению обратному гравитации, когда игрок не использует буст.
        if (!IsKeyDown(KEY_V))
            controlVector.y = Max(0, controlVector.y);

        const auto controlVector3 = Vector3(controlVector.x, 0, controlVector.y);

        auto d = gplayer.lookingDirection * (controlVector.y * gplayer.airSpeed * dt)
                 + axis * (controlVector.x * gplayer.airSpeed * dt);

        if (IsKeyDown(KEY_V)) {
            const auto t = GetTime();
            if (t - gplayer.lastBoostTime > gplayer.boostSoundInterval) {
                PlaySound(gdata.fxBoost);
                gplayer.lastBoostTime = t;
            }

            gplayer.buttonBoostPressedTime = t;
            d *= gplayer.boostAmount;
        }

        gplayer.velocity += d;
    }

    {  // gravity.
        gplayer.velocity.y += dt * gplayer.gravity;
    }

    {  // Dashing.
        if (IsMouseButtonPressed(1)) {
            auto l           = Vector3Length(gplayer.velocity);
            gplayer.velocity = gplayer.lookingDirection * l;

            gplayer.velocity += ApplyImpulse(
                gplayer.lookingDirection, gplayer.mass, gplayer.dashImpulse
            );

            gplayer.lastDashTime          = GetTime();
            gplayer.buttonDashPressedTime = GetTime();
            PlaySound(gdata.fxDash);
        }
    }

    SetSoundVolume(gdata.fxBoost, Vector3Length(gplayer.velocity) / gplayer.maxVelocity);

    // Выпускание / забирание троса.
    if (IsMouseButtonPressed(0)) {
        if (gplayer.ropeActivated) {
            gplayer.ropeActivated = false;
            PlaySound(gdata.fxGrappleBack);
            gplayer.buttonGrapplePressedTime = GetTime();
        }
        else if (gplayer.collided) {
            gplayer.ropeActivated = true;
            gplayer.ropePos       = gplayer.lookingAtCollision;
            gplayer.ropeLength    = Vector3Distance(gplayer.ropePos, gplayer.position);
            PlaySound(gdata.fxGrapple);
            gplayer.buttonGrapplePressedTime = GetTime();
        }
    }

    {  // Movement.
        // Decaying velocity.
        gplayer.velocity = Vector3ExponentialDecay(
            gplayer.velocity, Vector3Zero(), gplayer.velocityDecay, dt
        );

        // Clamping velocity.
        gplayer.velocity = Vector3Normalize(gplayer.velocity)
                           * Min(gplayer.maxVelocity, Vector3Length(gplayer.velocity));

        auto& position = gplayer.position;

        const auto oldPos = position;
        position += gplayer.velocity * dt;

        if (gplayer.ropeActivated) {
            auto& ropePos = gplayer.ropePos;

            float newDist = Vector3Distance(position, ropePos);

            bool newDistanceIsSufficient = newDist <= gplayer.ropeLength;
            if (!newDistanceIsSufficient) {
                position
                    = ropePos + Vector3Normalize(position - ropePos) * gplayer.ropeLength;

                gplayer.velocity = TransformVelocityBasedOnRopeDirection(
                    gplayer.velocity, ropePos - position
                );
            }
        }

        // Particles generation.
        if (IsKeyDown(KEY_V)) {
            float k = Vector3Length(gplayer.velocity) / gplayer.maxVelocity;

            int amountToGenerate = int(k * dt * gplayer.particlesAmountPerSecond) + 1;
            amountToGenerate     = Min(amountToGenerate, NUM_PARTICLES);

            float t = (float)GetTime();

            int startedIndex = gdata.nextToGenerateParticleIndex;
            FOR_RANGE (int, i, amountToGenerate) {
                int ii = gdata.nextToGenerateParticleIndex % NUM_PARTICLES;

                auto p
                    = Vector3Lerp(oldPos, position, float(i) / float(amountToGenerate));

                gdata.positions[ii]  = Vector4(p.x, p.y, p.z, 0);
                const float scale    = 0.2f;
                gdata.velocities[ii] = Vector4(
                    GetRandomFloat(-0.5, 0.5) * scale,
                    GetRandomFloat(-0.5, 0.5) * scale,
                    GetRandomFloat(-0.5, 0.5) * scale,
                    0
                );
                gdata.timesOfCreation[ii] = t;

                gdata.nextToGenerateParticleIndex++;
                if (gdata.nextToGenerateParticleIndex >= NUM_PARTICLES)
                    gdata.nextToGenerateParticleIndex -= NUM_PARTICLES;
            }
        }
    }

    {  // Переход в Grounded состояние.
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
            gdata.fxFootsteps[i]
                = LoadSound(TextFormat("resources/screens/gameplay/footstep_%i.wav", i));
        }
    }
    gdata.fxBoost       = LoadSound("resources/screens/gameplay/boost.wav");
    gdata.fxJump        = LoadSound("resources/screens/gameplay/jump.wav");
    gdata.fxDash        = LoadSound("resources/screens/gameplay/dash.wav");
    gdata.fxGrapple     = LoadSound("resources/screens/gameplay/grapple.wav");
    gdata.fxGrappleBack = LoadSound("resources/screens/gameplay/grappleBack.wav");

    if (gplayer.currentState == nullptr)
        gplayer.currentState = gdata.states + (int)PlayerStates::GROUNDED;

    gdata.finishScreen = 0;

    gdata.camera.target     = Vector3{0.0f, 0.0f, 0.0f};
    gdata.camera.up         = Vector3{0.0f, 1.0f, 0.0f};
    gdata.camera.fovy       = gplayer.defaultFov;
    gdata.camera.projection = CAMERA_PERSPECTIVE;

    {  // Particles.
        gdata.particleShader = LoadShader(
            "resources/screens/gameplay/particle_vertex.glsl",
            "resources/screens/gameplay/particle_fragment.glsl"
        );

        // Now we prepare the buffers that we connect to the shaders.
        // For each variable we want to give our particles, we create one buffer
        // called a Shader Storage Buffer Object containing a single variable type.
        //
        // We will use only Vector4 as particle variables, because data in buffers
        // requires very strict alignment rules.
        // You can send structs, but if not properly aligned will introduce many bugs.
        // For information on the std430 buffer layout see:
        // https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL).
        //
        // Number of particles should be a multiple of 1024, our workgroup size
        // (set in shader).
        gdata.positions       = (Vector4*)RL_MALLOC(sizeof(Vector4) * NUM_PARTICLES);
        gdata.velocities      = (Vector4*)RL_MALLOC(sizeof(Vector4) * NUM_PARTICLES);
        gdata.timesOfCreation = (float*)RL_MALLOC(sizeof(float) * NUM_PARTICLES);

        FOR_RANGE (int, i, NUM_PARTICLES) {
            gdata.positions[i]       = Vector4(0, 0, 0, 0);
            gdata.velocities[i]      = Vector4(0, 0, 0, 0);
            gdata.timesOfCreation[i] = -100.0f;
        }

        // Load three buffers: Position, Velocity and Starting Position.
        // Read/Write=RL_DYNAMIC_COPY.
        gdata.ssbo0 = rlLoadShaderBuffer(
            NUM_PARTICLES * sizeof(Vector4), gdata.positions, RL_DYNAMIC_COPY
        );
        gdata.ssbo1 = rlLoadShaderBuffer(
            NUM_PARTICLES * sizeof(Vector4), gdata.velocities, RL_DYNAMIC_COPY
        );
        gdata.ssbo2 = rlLoadShaderBuffer(
            NUM_PARTICLES * sizeof(float), gdata.timesOfCreation, RL_DYNAMIC_COPY
        );

        Assert(gdata.ssbo0 != 0);
        Assert(gdata.ssbo1 != 0);
        Assert(gdata.ssbo2 != 0);

        // For instancing we need a Vertex Array Object.
        // Raylib Mesh* is inefficient for millions of particles.
        // For info see: https://www.khronos.org/opengl/wiki/Vertex_Specification
        gdata.particleVao = rlLoadVertexArray();
        rlEnableVertexArray(gdata.particleVao);
        {
            Vector2 vertices[] = {
                {-1, -1},
                {1, -1},
                {-1, 1},
            };
            rlEnableVertexAttribute(0);
            rlLoadVertexBuffer(vertices, sizeof(vertices), false);  // dynamic=false
            rlSetVertexAttribute(0, 2, RL_FLOAT, false, 0, 0);
        }
        rlDisableVertexArray();

        // gdata.particleVao2 = rlLoadVertexArray();
        // rlEnableVertexArray(gdata.particleVao2);
        // {
        //     Vector2 vertices[] = {
        //         {-1, 1},
        //         {1, -1},
        //         {1, 1},
        //     };
        //     rlEnableVertexAttribute(0);
        //     rlLoadVertexBuffer(vertices, sizeof(vertices), false);  // dynamic=false
        //     rlSetVertexAttribute(0, 2, RL_FLOAT, false, 0, 0);
        // }
        // rlDisableVertexArray();
    }
    // ------------------------------------------------------------

    {  // Loading level.
        char* data = LoadFileText("resources/screens/gameplay/level.txt");

        std::istringstream iss(data);

        int colorsCount = 0;
        iss >> colorsCount;
        gdata.colors.reserve(colorsCount);

        FOR_RANGE (int, i, colorsCount) {
            int r, g, b = 0;
            iss >> r;
            iss >> g;
            iss >> b;
            gdata.colors.push_back(Color{
                (unsigned char)r,
                (unsigned char)g,
                (unsigned char)b,
                255,
            });
        }

        int cubesCount = 0;
        iss >> cubesCount;
        gdata.cubes.reserve(cubesCount);

        FOR_RANGE (int, i, cubesCount) {
            int x, y, z, colorIndex = 0;
            iss >> x;
            iss >> y;
            iss >> z;
            iss >> colorIndex;
            gdata.cubes.push_back({x, y, z, colorIndex});
        }

        UnloadFileText(data);
    }

    DisableCursor();
}

// Gameplay Screen Update logic.
void UpdateGameplayScreen() {
    const auto dt = GetFrameTime();

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
        if (IsKeyPressed(KEY_F3)) {
            gplayer.buttonClearPathsPressedTime = GetTime();
            gdata.linesToDraw.clear();
            gdata.colorsOfLines.clear();
        }
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

        FOR_RANGE (int, i, gdata.cubes.size()) {
            const auto& cube = gdata.cubes[i];

            const auto cubePos
                = Vector3((float)cube.pos.x, (float)cube.pos.y, (float)cube.pos.z);
            BoundingBox box = {cubePos, cubePos + Vector3One()};

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

    // {  // Particles. Compute pass.
    //     float time      = (float)GetTime();
    //     float timeScale = 3.2f;
    //
    //     rlEnableShader(gdata.particleComputeShader);
    //
    //     // Set our parameters. The indices are set in the shader.
    //     rlSetUniform(0, &time, SHADER_UNIFORM_FLOAT, 1);
    //     rlSetUniform(1, &timeScale, SHADER_UNIFORM_FLOAT, 1);
    //     rlSetUniform(2, &dt, SHADER_UNIFORM_FLOAT, 1);
    //
    //     rlBindShaderBuffer(gdata.ssbo0, 0);
    //     rlBindShaderBuffer(gdata.ssbo1, 1);
    //     // rlBindShaderBuffer(gdata.ssbo2, 2);
    //
    //     // We have NUM_PARTICLES/1024 workGroups. Each workgroup has size 1024.
    //     rlComputeShaderDispatch(NUMBER_OF_INSTANCES, 1, 1);
    //     rlDisableShader();
    // }

    FOR_RANGE (int, i, NUM_PARTICLES) {
        gdata.positions[i] += gdata.velocities[i] * dt;
    }

    // TODO: qsort
    {  // Сортировка частиц от точки "взора" игрока до них.
        const auto eyePos = gplayer.position + Vector3Up * 2.0f;

        FOR_RANGE (int, i, NUM_PARTICLES - 1) {
            bool swapped = false;

            FOR_RANGE (int, j, NUM_PARTICLES - i - 1) {
                auto& pos1 = gdata.positions[j];
                auto& pos2 = gdata.positions[j + 1];

                float d1 = Vector3Distance(eyePos, Vector3(pos1.x, pos1.y, pos1.z));
                float d2 = Vector3Distance(eyePos, Vector3(pos2.x, pos2.y, pos2.z));

                if (d2 > d1) {
                    auto& vel1 = gdata.velocities[j];
                    auto& vel2 = gdata.velocities[j + 1];
                    auto& toc1 = gdata.timesOfCreation[j];
                    auto& toc2 = gdata.timesOfCreation[j + 1];

                    std::swap(pos1, pos2);
                    std::swap(vel1, vel2);
                    std::swap(toc1, toc2);

                    swapped = true;
                }
            }
            if (!swapped)
                break;
        }
    }
    UpdateSSBOAsRingBuffer(
        gdata.ssbo0, (void*)gdata.positions, sizeof(Vector4), 0, 0, NUM_PARTICLES
    );
    UpdateSSBOAsRingBuffer(
        gdata.ssbo1, (void*)gdata.velocities, sizeof(Vector4), 0, 0, NUM_PARTICLES
    );
    UpdateSSBOAsRingBuffer(
        gdata.ssbo2, (void*)gdata.timesOfCreation, sizeof(float), 0, 0, NUM_PARTICLES
    );
}

// Gameplay Screen Draw logic.
void DrawGameplayScreen() {
    DebugTextReset();

    const auto dt = GetFrameTime();

    const auto    screenWidth  = GetScreenWidth();
    const auto    screenHeight = GetScreenHeight();
    const Vector2 screenSize{(float)screenWidth, (float)screenHeight};

    DrawRectangle(0, 0, screenWidth, screenHeight, BLACK);

    auto& camera    = gdata.camera;
    camera.position = gplayer.position + Vector3Up * 2.0f;
    camera.target   = camera.position + gplayer.lookingDirection * 100.0f;

    {  // FOV.
        const float dashElapsed          = (float)(GetTime() - gplayer.lastDashTime);
        const bool  dashAnimationExpired = dashElapsed
                                          > (gplayer.fromDefaultToDashFovDuration
                                             + gplayer.fromDashToDefaultFovDuration);

        if (dashAnimationExpired) {
            camera.fovy = gplayer.defaultFov;
        }
        else {
            if (dashElapsed < gplayer.fromDefaultToDashFovDuration) {
                camera.fovy = Clamp(
                    Lerp(
                        gplayer.defaultFov,
                        gplayer.dashedFov,
                        dashElapsed / gplayer.fromDefaultToDashFovDuration
                    ),
                    Min(gplayer.defaultFov, gplayer.dashedFov),
                    Max(gplayer.defaultFov, gplayer.dashedFov)
                );
            }
            else {
                camera.fovy = Clamp(
                    Lerp(
                        gplayer.dashedFov,
                        gplayer.defaultFov,
                        (dashElapsed - gplayer.fromDefaultToDashFovDuration)
                            / gplayer.fromDashToDefaultFovDuration
                    ),
                    Min(gplayer.defaultFov, gplayer.dashedFov),
                    Max(gplayer.defaultFov, gplayer.dashedFov)
                );
            }
        }
    }

    BeginMode3D(camera);
    {  // Drawing world.
        FOR_RANGE (int, i, gdata.cubes.size()) {
            const auto& cube = gdata.cubes[i];
            const auto& pos
                = Vector3((float)cube.pos.x, (float)cube.pos.y, (float)cube.pos.z);
            DrawCubeV(
                pos + Vector3One() / 2.0f, Vector3One(), gdata.colors[cube.colorIndex]
            );
            DrawCubeWiresV(pos + Vector3One() / 2.0f, Vector3One(), BLACK);
        }
    }

    DrawGrid(100, 1.0f);

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
    EndMode3D();

    BeginMode3D(camera);
    {  // Particles. Drawing pass.
        const float particleScale = 100.0;
        const auto  NUM_PARTICLES = NUMBER_OF_INSTANCES * PARTICLES_PER_SHADER_INSTANCE;

        rlEnableShader(gdata.particleShader.id);

        // Because we use rlgl, we must take care of matrices ourselves.
        // We need to only pass the projection and view matrix.
        // These will be used to make the particle face the camera and such.
        Matrix projection = rlGetMatrixProjection();
        Matrix view       = GetCameraMatrix(camera);
        float  time       = (float)GetTime();

        SetShaderValueMatrix(gdata.particleShader, 0, projection);
        SetShaderValueMatrix(gdata.particleShader, 1, view);
        SetShaderValue(gdata.particleShader, 2, &particleScale, SHADER_UNIFORM_FLOAT);
        SetShaderValue(gdata.particleShader, 3, &time, SHADER_UNIFORM_FLOAT);

        rlBindShaderBuffer(gdata.ssbo0, 0);
        rlBindShaderBuffer(gdata.ssbo1, 1);
        rlBindShaderBuffer(gdata.ssbo2, 2);

        // Particles drawing. Instancing will duplicate the vertices.
        {
            rlEnableVertexArray(gdata.particleVao);
            rlDrawVertexArrayInstanced(0, 3, 2 * NUM_PARTICLES);
            rlDisableVertexArray();
        }
        rlDisableShader();
    }

    if (gdata.gizmosEnabled) {  // Drawing lines 3D.
        FOR_RANGE (int, i, gdata.linesToDraw.size() / 2) {
            auto& p1 = gdata.linesToDraw[i * 2];
            auto& p2 = gdata.linesToDraw[i * 2 + 1];
            DrawLine3D(p1, p2, gdata.colorsOfLines[i]);
        }
        // gdata.linesToDraw.clear();
    }
    EndMode3D();

    {  // Cross.
        const int size  = 20;
        const int width = 4;

        auto color = WHITE;
        if (gplayer.collided)
            color = GREEN;
        if (gplayer.ropeActivated)
            color = RED;

        DrawRectangle(
            screenWidth / 2 - size / 2, screenHeight / 2 - width / 2, size, width, color
        );
        DrawRectangle(
            screenWidth / 2 - width / 2, screenHeight / 2 - size / 2, width, size, color
        );

        DrawRectangleLines(
            screenWidth / 2 - size / 2, screenHeight / 2 - width / 2, size, width, BLACK
        );
        DrawRectangleLines(
            screenWidth / 2 - width / 2, screenHeight / 2 - size / 2, width, size, BLACK
        );
    }

    // DebugTextDraw(
    //     TextFormat("FPS: %i (press F1 to change)",
    //     fpsValues[gdata.currentFPSValueIndex])
    // );
    // DebugTextDraw("Toggle gizmos - F2");
    DebugTextDraw(TextFormat(
        "pos %.2f %.2f %.2f", gplayer.position.x, gplayer.position.y, gplayer.position.z
    ));
    // DebugTextDraw(TextFormat(
    //     "vel (%.2f) %.2f %.2f %.2f",
    //     Vector3Length(gplayer.velocity),
    //     gplayer.velocity.x,
    //     gplayer.velocity.y,
    //     gplayer.velocity.z
    // ));
    DebugTextDraw(TextFormat(
        "look %.2f %.2f %.2f",
        gplayer.lookingDirection.x,
        gplayer.lookingDirection.y,
        gplayer.lookingDirection.z
    ));
    // DebugTextDraw(TextFormat("rope length %.2f", gplayer.ropeLength));
    // DebugTextDraw(TextFormat("fov %.2f", camera.fovy));

    bool isAirborne
        = gplayer.currentState == (gdata.states + (int)PlayerStates::AIRBORNE);

    ButtonTextDraw("SPACE - Jump", &gplayer.buttonJumpPressedTime, !isAirborne);
    ButtonTextDraw("LMB - Grapple", &gplayer.buttonGrapplePressedTime, isAirborne);
    ButtonTextDraw("RMB - Dash", &gplayer.buttonDashPressedTime, isAirborne);
    ButtonTextDraw("V - Apply Boost", &gplayer.buttonBoostPressedTime, isAirborne);
    // ButtonTextDraw("F3 - Clear Gizmos", &gplayer.buttonClearPathsPressedTime);
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
    UnloadSound(gdata.fxDash);
    UnloadSound(gdata.fxGrapple);
    UnloadSound(gdata.fxGrappleBack);
    UnloadSound(gdata.fxBoost);

    UnloadShader(gdata.particleShader);
    // rlUnloadShaderProgram(gdata.particleComputeShader);
    // gdata.particleComputeShader = 0;

    RL_FREE(gdata.positions);
    RL_FREE(gdata.velocities);
    RL_FREE(gdata.timesOfCreation);
    gdata.positions       = nullptr;
    gdata.velocities      = nullptr;
    gdata.timesOfCreation = nullptr;
}

// Gameplay Screen should finish?
int FinishGameplayScreen() {
    return gdata.finishScreen;
}
