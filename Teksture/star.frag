#version 330 core

uniform float time; // Vreme za oscilovanje
uniform vec3 dynamicColor; // Boja neba

out vec4 outCol;

void main()
{
    float intensity = (sin(time * 50.0f) + 1.0f) / 2.0f;
    vec3 starColor = dynamicColor + intensity * (vec3(1.0) - dynamicColor); // Od boje neba ka beloj
    outCol = vec4(starColor, 1.0);
}