#version 330 core

layout(location = 0) in vec2 inPos;

uniform float time;       // Trenutno vreme u sekundama
uniform float duration;   // Trajanje �irenja (3�5 sekundi)
uniform float initialSize; // Po�etna veli�ina kruga (npr. 0.2f)
uniform float finalSize;   // Krajnja veli�ina kruga (npr. 0.4f)

void main()
{
    float progress = clamp(time / duration, 0.0, 1.0);

    // Interpolacija veli�ine (linearno �irenje)
    float scale = initialSize + (finalSize - initialSize) * progress;

    // Skaliranje pozicija ta�aka za �irenje kruga
    vec2 scaledPos = inPos * scale;

    // Transformisana pozicija
    gl_Position = vec4(inPos, 0.0, 1.0);
}