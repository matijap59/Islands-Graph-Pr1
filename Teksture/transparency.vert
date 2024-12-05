#version 330 core 

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTex; //Koordinate texture, propustamo ih u FS kao boje
layout(location = 2) in float isTopVertex;

uniform vec2 uPos;
uniform float seaHeight;
uniform bool isIsland;
uniform bool isFire;
uniform float time;

out vec2 chTex;

void main()
{
	vec2 pos = inPos;
    
    if(isIsland && !isFire){
        if(inPos.y > -0.41){
            gl_Position = vec4(inPos.x, inPos.y - seaHeight / 5.0, 0.0, 1.0);
        } else if(inPos.y < 0.0){
            gl_Position = vec4(inPos.x, inPos.y + seaHeight / 5.0, 0.0, 1.0);
        }
    }

    if(isIsland && isFire){
        if(isTopVertex > 0.0) {
            // Manipulacija gornjeg temena za animaciju (ako je to vrh)
            pos.y += sin(time) * 0.1; // Animacija pozicije
        } else {
            // Ostali vrhovi ostaju u odnosu na ostrvo
            if(inPos.y > -0.41) {
                pos.y -= seaHeight / 5.0;
            } else if(inPos.y < 0.0) {
                pos.y += seaHeight / 5.0;
            }
        }
        if (isTopVertex == 0.0) {
                float stretchFactor = sin(time) * 0.02; // Amplituda širenja
                if (inPos.x < 0.0) {
                    // Leva ivica se pomera ulevo
                    pos.x += stretchFactor;
                } else {
                    // Desna ivica se pomera udesno
                    pos.x -= stretchFactor;
                }
        }
        gl_Position = vec4(pos, 0.0, 1.0);
    }

    chTex = inTex;
}