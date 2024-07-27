#version 430

layout (location=3) uniform float currentTime;

in vec4 fragColor;
in float timeOfCreation;

out vec4 finalColor;

void main()
{
    float threshold = 1.0f;
    // if ((currentTime - timeOfCreation) > threshold)
    //     discard;

    finalColor = fragColor;
    // finalColor.a = (currentTime - timeOfCreation) / threshold;
}
