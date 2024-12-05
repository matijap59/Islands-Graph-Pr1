#version 330 core

in vec4 chCol;
out vec4 outCol;

uniform vec3 dynamicColor; // Uniform za dinami�ku boju

void main()
{
    outCol = vec4(dynamicColor, 1.0); // Koristite prosle�enu boju
}