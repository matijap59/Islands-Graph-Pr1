#version 330 core

in vec4 chCol;
out vec4 outCol;
uniform bool transparencySea;

void main()
{
	float a  = 0.0;
	if(transparencySea){
		a=0.1;
	}else{
		a=1.0;
	}
	outCol = vec4(chCol.rgb, a);
	//outCol = chCol;
}