#version 430

in vec4 fragColor;
in vec4 dur;

out vec4 finalColor;

void main()
{
    const float opaqueDuration = 2;
    const float fadeDuration = 4;
    finalColor = fragColor;

    if (dur.x > (opaqueDuration + fadeDuration))
        discard;
    else if (dur.x > opaqueDuration)
        finalColor.a = 1 - (dur.x - opaqueDuration) / fadeDuration;
    else
        finalColor.a = 1;
}
