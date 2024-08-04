#version 430

in vec4  fragColor;
in float particleLivingDuration;
in vec2 coord;

out vec4 finalColor;

void main()
{
    const float opaqueDuration = 8;
    const float fadeDuration = 6;
    finalColor = fragColor;
    finalColor.rgb = vec3(1, 1, 1);

    const float dur = particleLivingDuration;

    if (dur > (opaqueDuration + fadeDuration))
        discard;
    else if (dur > opaqueDuration)
        finalColor.a = 1 - (dur - opaqueDuration) / fadeDuration;
    else
        finalColor.a = 1;

    const float d = sqrt(coord.x * coord.x + coord.y * coord.y);
    if (d > 1)
        discard;

    finalColor.a *= 1 - d;
}
