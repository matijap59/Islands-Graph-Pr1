#version 330 core 

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec4 inCol;
out vec4 chCol;			//boja koja ide u fragment

uniform float seaHeight;
uniform bool isIsland;
uniform bool isFire;
uniform float moveX;  // Pomak na X osi
uniform float moveY;
//uniform bool isCircle;

void main()
{
	if(isIsland && !isFire){
		if(inPos.y > -0.41){
			gl_Position = vec4(inPos.x+moveX, inPos.y-seaHeight/5+moveY, 0.0, 1.0);
		}else{
			if(inPos.y < 0.0){
				gl_Position = vec4(inPos.x+moveX, inPos.y+seaHeight/5+moveY, 0.0, 1.0);
			}
		}
	}else{
		if(inPos.y > -0.5){
			gl_Position = vec4(inPos.x+moveX, inPos.y+seaHeight+moveY, 0.0, 1.0);
		}else{
			if(inPos.y < 0.5){
				gl_Position = vec4(inPos.x+moveX, inPos.y - seaHeight+moveY, 0.0, 1.0);
			}
		}
	}
	if(isIsland && isFire){
		if(inPos.y > -0.41){
			gl_Position = vec4(inPos.x+moveX, inPos.y-seaHeight/5+moveY, 0.0, 1.0);
		}else{
			if(inPos.y < 0.0){
				gl_Position = vec4(inPos.x+moveX, inPos.y+seaHeight/5+moveY, 0.0, 1.0);
			}
		}
	}
	chCol = inCol;
}