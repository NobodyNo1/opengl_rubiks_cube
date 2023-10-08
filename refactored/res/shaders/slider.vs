#version 330 core
layout (location = 0) in vec3 aPos;
out vec3 ourColor;

uniform vec3 sliderColor;
uniform vec2 iResolution;
uniform mat4 slModel;

void main()
{
    float newXCoord = aPos.x*iResolution.y/iResolution.x;
    vec3 uvCood = vec3(newXCoord, aPos.y, aPos.z);
    gl_Position = slModel*vec4(uvCood, 1.0);
    ourColor = sliderColor;
}