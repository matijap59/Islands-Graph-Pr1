#version 330 core

in vec4 chCol;
in vec2 fragPos;
in vec2 fireCenter;
out vec4 outCol;

uniform float time;

void main()
{
	float distance = length(fragPos - fireCenter);
	float r = (sin(time) + 1.0) / 2.0; // Red
    float g = (sin(time + 1.57) + 1.0) / 2.0; // Green
    float b = 0.0;
	//outCol = chCol;
	//outCol=vec4(r,g,b,1.0);
	//outCol=vec4(fireColor * intensity + ambientLight, 1.0);


	float lightIntensity = max(0.0, 1.0 - distance * 0.1); // Slabi svetlo sa udaljenošæu
	vec3 fireColor = vec3(r, g, b);
    vec3 ambientLight = vec3(0.5, 0.5, 0.5); // Ambijentalna svetlost koja je stalna
    outCol = vec4(fireColor * lightIntensity + ambientLight, 1.0);
}