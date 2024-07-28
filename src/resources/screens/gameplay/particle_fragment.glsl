#version 430

in vec4  fragColor;
in float particleLivingDuration;

out vec4 finalColor;

void main()
{
    const float opaqueDuration = 2;
    const float fadeDuration = 4;
    finalColor = fragColor;

    const float dur = particleLivingDuration;

    if (dur > (opaqueDuration + fadeDuration))
        discard;
    else if (dur > opaqueDuration)
        finalColor.a = 1 - (dur - opaqueDuration) / fadeDuration;
    else
        finalColor.a = 1;
}
