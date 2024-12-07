#version 330 core 

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec4 inCol;
layout(location = 2) in float isTopVertex;
out vec2 fragPos;
out vec4 chCol;            //boja koja ide u fragment
out vec2 fireCenter; 

uniform float seaHeight;
uniform bool isIsland;
uniform bool isFire;
uniform float time;

void main()
{
    vec2 pos = inPos;
    if(isIsland && isFire){
        if (isTopVertex > 0.0) {
            // Manipulacija gornjeg temena
            pos.y += sin(time) * 0.1; // Animacija
            //firCenter.y +=sin(time) * 0.1;
        }
        gl_Position = vec4(pos, 0.0, 1.0);
    }
    //fireCenter = pos;
    fragPos = pos;
    chCol = inCol;
}