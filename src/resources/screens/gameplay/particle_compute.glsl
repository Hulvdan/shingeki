//
// NOTE: Оставил комменты из примера.
// ref: https://github.com/arceryz/raylib-gpu-particles/blob/master/Shaders/particle_compute.glsl
//

// Version 430 supports compute shaders.
#version 430

// This is the workgroup size. The largest size that is guaranteed by OpenGL
// to be available is 1024, beyond this is uncertain.
// Might influence performance but only in advanced cases.
layout (local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

//
// Shader Storage Buffer Objects (SSBO's) can store any data and are
// declared like structs. There can only be one variable-sized array
// in an SSBO, for this reason we use multiple SSBO's to store our arrays.
//
// We do not use structures to store the particles, because an SSBO
// only has a guaranteed max size of 128 Mb.
// With 1 million particles you can store only 32 floats per particle in one SSBO.
// Therefore use multiple buffers since this example scales up to the millions.
//
layout(std430, binding=0) buffer ssbo0 { vec4 positions[]; };
layout(std430, binding=1) buffer ssbo1 { vec4 velocities[]; };

// Uniform values are the way in which we can modify the shader efficiently.
// These can be updated every frame efficiently.
// We use layout(location=...) but you can also leave it and query the location in Raylib.
layout(location=0) uniform float time;
layout(location=1) uniform float timeScale;
layout(location=2) uniform float dt;

void main() {
    uint index = gl_GlobalInvocationID.x;
    positions[index].xyz += velocities[index].xyz * (dt * timeScale);
}
