#version 330 core 

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec4 inCol;
out vec4 chCol;			//boja koja ide u fragment

uniform float seaHeight;
uniform bool isIsland;
uniform bool isFire;

void main()
{
	vec2 newPosition = inPos;
	gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0);
	if(inPos.y<-0.4){
		newPosition.y += seaHeight/5;
	}
	gl_Position = vec4(newPosition, 0.0, 1.0);
	chCol = inCol;
}