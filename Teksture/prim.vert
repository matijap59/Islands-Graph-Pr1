#version 330 core

layout(location = 0) in vec2 inPos;  // Pozicija
layout(location = 1) in vec4 inColVec; // Boja (opciono)

out vec4 colVec; // Prosleðivanje fragment šejderu

void main()
{
    gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0);
    colVec = inColVec; // Prosleðuje boju dalje
}