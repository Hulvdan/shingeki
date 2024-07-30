#version 430

// layout (location=4) uniform sampler2D texture;

in vec4  fragColor;
in float particleLivingDuration;
in vec3 coord;

out vec4 finalColor;

void main()
{
    const float opaqueDuration = 4;
    const float fadeDuration = 3;
    finalColor = fragColor;
    finalColor.rgb = vec3(1, 1, 1);

    const float dur = particleLivingDuration;

    if (dur > (opaqueDuration + fadeDuration))
        discard;
    else if (dur > opaqueDuration)
        finalColor.a = 1 - (dur - opaqueDuration) / fadeDuration;
    else
        finalColor.a = 1;

    const float dx = 2.0 / 3.0 - coord.x;
    const float dy = 2.0 / 3.0 - coord.y;
    const float dz = 2.0 / 3.0 - coord.z;

    const float dSquared  = dx * dx + dy * dy + dz * dz;
    const float dSquaredX =           dy * dy + dz * dz;
    const float dSquaredY = dx * dx +           dz * dz;
    const float dSquaredZ = dx * dx + dy * dy          ;
    const float d = sqrt(dSquared);
    if (d > 2.0 / 3.0)
        discard;

    finalColor.a *= min(min(sqrt(dSquaredX), sqrt(dSquaredY)), sqrt(dSquaredZ));
}
