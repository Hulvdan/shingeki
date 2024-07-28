// Version 430 supports compute shaders.
#version 430

// This is the workgroup size. The largest size that is guaranteed by OpenGL
// to be available is 1024, beyond this is uncertain.
// Might influence performance but only in advanced cases.
layout(local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding=0) buffer ssbo0 { vec4 positions[]; };
layout(std430, binding=1) buffer ssbo1 { vec4 velocities[]; };
// layout(std430, binding=2) buffer ssbo2 { vec4 startPositions[]; };

layout(location=0) uniform float time;
layout(location=1) uniform float timeScale;
layout(location=2) uniform float deltaTime;

const float PI = 3.14159;

void main()
{
    uint index = gl_GlobalInvocationID.x;

    positions[index].xyz += velocities[index].xyz * (deltaTime * timeScale);
}
