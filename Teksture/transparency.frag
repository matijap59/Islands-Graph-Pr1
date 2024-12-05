#version 330 core

in vec2 chTex; //koordinate teksture
out vec4 outCol;

uniform sampler2D uTex; //teksturna jedinica
uniform vec3 dynamicColor;

uniform float time;

void main()
{
    vec4 color = texture(uTex, chTex);
    
     if (color.r < 0.05 && color.g < 0.05 && color.b < 0.05) {
        discard; // Odbaci ovaj piksel
    }

    float noise = fract(sin(dot(gl_FragCoord.xy, vec2(12.9898, 78.233))) * 43758.5453123);
    float alphaVariation = smoothstep(0.2, 0.8, fract(noise + time));


    float diff = abs(sin(time))/5.0f;

    vec4 desiredColor = vec4(0.3-diff/5.0f, 0.0, 0.8+diff, 0.8); // RGB (0.5, 0, 1) + alfa 1.0

    vec4 baseColor = mix(vec4(1.0, 1.0, 1.0, 0.7), vec4(0.3, 0.3, 0.3, 0.5), alphaVariation);

    
    // Pomnoži boje (teksture * željena boja)
    //outCol = color * desiredColor;
    outCol = color * desiredColor + baseColor*0.5; 

}