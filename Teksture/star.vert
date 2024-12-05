#version 330 core

layout(location = 0) in vec2 inPos;  // Pozicija zvezde
layout(location = 1) in float inSize; // Velièina zvezde

void main()
{
    gl_Position = vec4(inPos, 0.0, 1.0);
    gl_PointSize = inSize * 1000.0; // Skaliranje velièine
}