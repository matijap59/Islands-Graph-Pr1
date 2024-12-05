#version 330 core 

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTex; //Koordinate texture, propustamo ih u FS kao boje
uniform float seaHeight;
uniform bool isIsland;
uniform bool isFire;
out vec2 chTex;

void main()
{
	if(isIsland && !isFire){
		if(inPos.y > -0.41){
			gl_Position = vec4(inPos.x, inPos.y-seaHeight/5, 0.0, 1.0);
		}else{
			if(inPos.y < 0.0){
				gl_Position = vec4(inPos.x, inPos.y+seaHeight/5, 0.0, 1.0);
			}
		}
	}else{
		if(inPos.y > -0.5){
			gl_Position = vec4(inPos.x, inPos.y+seaHeight, 0.0, 1.0);
		}else{
			if(inPos.y < 0.5){
				gl_Position = vec4(inPos.x, inPos.y - seaHeight, 0.0, 1.0);
			}
		}
	}
	if(isIsland && isFire){
		if(inPos.y > -0.41){
			gl_Position = vec4(inPos.x, inPos.y-seaHeight/5, 0.0, 1.0);
		}else{
			if(inPos.y < 0.0){
				gl_Position = vec4(inPos.x, inPos.y+seaHeight/5, 0.0, 1.0);
			}
		}
	}
	chTex = inTex;
}