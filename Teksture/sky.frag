#version 330 core

in vec4 chCol;
out vec4 outCol;

uniform vec3 dynamicColor; // Uniform za dinamièku boju

void main()
{
    outCol = vec4(dynamicColor, 1.0); // Koristite prosleðenu boju
}