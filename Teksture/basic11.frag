#version 330 core

in vec4 chCol;
out vec4 outCol;

void main()
{
	outCol = vec4(chCol.rgb, 0.5);
}