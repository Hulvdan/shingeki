#version 430

in vec4 fragColor;
// in float timeOfCreation;
// in float currentTime;

out vec4 finalColor;

void main()
{
    float threshold = 1.0f;
    // if ((currentTime - timeOfCreation) > threshold)
    //     discard;

    finalColor = vec4(1,0,0,1);
    // finalColor = fragColor;
    // finalColor.a = (currentTime - timeOfCreation) / threshold;
}
