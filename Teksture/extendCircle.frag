#version 330 core

uniform float time;       // Trenutno vreme u sekundama
uniform float duration;   // Trajanje širenja (3–5 sekundi)

out vec4 outCol;

void main()
{
    // Provera da li je vreme isteklo
    //if (time > duration) {
    //    discard; // Ignoriši fragmente, èime se krug ne crta
    //} else {
    //    outCol = vec4(1.0, 0.0, 0.0, 0.5); // Poluprovidna crvena boja
    //}
    outCol = vec4(1.0, 0.0, 0.0, 0.5);
}