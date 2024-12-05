//Autor: Nedeljko Tesanovic
//Opis: Primjer upotrebe tekstura

#define _CRT_SECURE_NO_WARNINGS
#define CRES 60 // Circle Resolution = Rezolucija kruga

#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <thread>
#include <chrono>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
static unsigned loadImageToTexture(const char* filePath); //Ucitavanje teksture, izdvojeno u funkciju
static void mouse_callback(GLFWwindow* window, double x, double y);
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void initializeRedCircle(double xCircleCenter, double yCircleCenter);
GLFWcursor* createCustomCursor(const char* imagePath, int desiredWidth, int desiredHeight);
unsigned char* loadImage(const char* path, int& width, int& height, int& channels);
unsigned char* resizeImage(unsigned char* inputImage, int inputWidth, int inputHeight, int channels, int outputWidth, int outputHeight);
void initScreenDimensions();


void initializeHelp();

bool isPointInTriangle(float px, float py,
    float ax, float ay,
    float bx, float by,
    float cx, float cy);

bool mouseInPosition = false;
bool mousePressed = false;
bool activateFire = false;
bool activateRedCircle = false;

float screenWidth, screenHeight;

static float startTime = 0.0f;
static bool isFirstUpdate = true;
double lastSpacePressTime = 0.0;
double lastSpacePressTimeR = 0.0;
double lastSpacePressTimeM = 0.0;
double lastSpacePressTimeP = 0.0;

const int TARGET_FPS = 60;
const double FRAME_TIME = 1.0 / TARGET_FPS;

int trashold_frame = 50;
float y_offset = 0.0f; // Poèetni offset

std::string textPOMOC = "ÆOMOP";
int global_counter = 0;
int counterr = 0;
std::string current_text = "P";

int counter_strings = 0;

unsigned int VAO[100];
unsigned int VBO[103];

unsigned int EBO;
unsigned int EBOClouds;
unsigned int EBOPalm;
unsigned int EBOFireTrans;
float rr = 0.05f;


unsigned int stride = (6) * sizeof(float);

unsigned int strideTexture = (2 + 2) * sizeof(float);

unsigned int strideTextureFire = (5) * sizeof(float);

float xCircleCenter=0.0f;
float yCircleCenter=0.0f;

float centerXR;
float centerYR;

float redCircle[(CRES + 2) * 2];
float aspectRatio = screenWidth / screenHeight;


struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

int main(void)
{

    if (!glfwInit())
    {
        std::cout<<"GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    unsigned int wWidth = 1920;
    unsigned int wHeight = 1080;
    const char wTitle[] = "[Generic Title]";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, glfwGetPrimaryMonitor(), NULL);
    //window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);
    initScreenDimensions();

    std::cout << screenWidth << std::endl;
    std::cout << screenHeight << std::endl;

    
    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    std::map<char, Character> Characters;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, "Fonts/arial.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    unsigned int textures[128];
    glGenTextures(128, textures);
    for (int i = 0; i < 128; i++) {
        std::cout << "TEXTURE : " << textures[i] << std::endl;
    }
    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture = textures[c];
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        std::cout << "Texture ID for character " << c << ": " << texture << std::endl;
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        std::cout << "Glyph Width: " << face->glyph->bitmap.width
            << ", Height: " << face->glyph->bitmap.rows << std::endl;
        Characters.insert(std::pair<char, Character>(c, character));
    }


    float waterLevel = 0.0f;
    float skyHeight = 1.0f;

    float skyVertices[] = {
        -1.0f,  skyHeight,  0.0f, 0.0f, 1.0f, 1.0f,  // Gornji levi
         1.0f,  skyHeight,  0.0f, 0.0f, 1.0f, 1.0f,  // Gornji desni
        -1.0f, -1.0f,  0.0f, 0.0f, 1.0f, 1.0f,  // Donji levi
         1.0f, -1.0f,  0.0f, 0.0f, 1.0f, 1.0f   // Donji desni
    };

    // Kada se nivo vode promeni, ažuriraj visinu neba
    skyHeight = 1.0f + waterLevel;  // Poveæaj visinu neba kada nivo vode raste

    // Zatim prikaži i vodu
    float seaVertices[] = {
        -1.0f, -1.0f,  0.5f, 0.0f, 1.0f, 1.0f,  // Donji levi
         1.0f, -1.0f,  0.5f, 0.0f, 1.0f, 1.0f,  // Donji desni
        -1.0f, waterLevel,  0.5f, 0.0f, 1.0f, 1.0f,  // Gornji levi (na nivou vode)
         1.0f, waterLevel,  0.5f, 0.0f, 1.0f, 1.0f   // Gornji desni (na nivou vode)
    };

    float topWidth = 0.4f; // širina gornje strane
    float bottomWidth = 0.6f; // širina donje strane
    float height = 0.3f; // visina trapeza
    float centerX = 0.0f; // X koordinata centra trapeza
    float centerY = 0.0f; // Y koordinata centra trapeza
    float smallIslandWidth = 0.1f;  // širina manjeg ostrva
    float smallIslandHeight = 0.2f; // visina manjeg ostrva
    float offset = 0.3f;
    float offset1 = 0.1f;

    // Definisanje koordinata trapeza
    float islandVertices[] = {
        centerX - topWidth / 2, centerY + height / 2 - offset, 1.0f, 1.0f, 0.0f, 1.0f,   // Gornji levi
        centerX + topWidth / 2, centerY + height / 2 - offset, 1.0f, 1.0f, 0.0f, 1.0f,   // Gornji desni
        centerX - bottomWidth / 2, centerY - height / 2 - offset,1.0f, 1.0f, 0.0f, 1.0f,  // Donji levi
        centerX + bottomWidth / 2, centerY - height / 2 - offset, 1.0f, 1.0f, 0.0f, 1.0f,  // Donji desni

        centerX + 0.5f - smallIslandWidth / 2+offset1, centerY - 0.5f + smallIslandHeight / 2, 1.0f, 1.0f, 0.0f, 1.0f, // Gornji levi
        centerX + 0.5f + smallIslandWidth / 2 + offset1, centerY - 0.5f + smallIslandHeight / 2, 1.0f, 1.0f, 0.0f, 1.0f, // Gornji desni
        centerX + 0.5f - smallIslandWidth / 2 + offset1-0.1, centerY - 0.5f - smallIslandHeight / 2, 1.0f, 1.0f, 0.0f, 1.0f, // Donji levi
        centerX + 0.5f + smallIslandWidth / 2 + offset1+0.1, centerY - 0.5f - smallIslandHeight / 2, 1.0f, 1.0f, 0.0f, 1.0f, // Donji desni

        // Manje ostrvo 2 (pomereno u levo)
        centerX - 0.5f - smallIslandWidth / 2 - offset/2, centerY - 0.5f + smallIslandHeight / 2 + offset / 4 , 1.0f, 1.0f, 0.0f, 1.0f, // gornji levi
        centerX - 0.5f + smallIslandWidth / 2 - offset / 2, centerY - 0.5f + smallIslandHeight / 2 + offset / 4, 1.0f, 1.0f, 0.0f, 1.0f, // gornji desni
        centerX - 0.5f - smallIslandWidth / 2 - offset / 2-0.1, centerY - 0.5f - smallIslandHeight / 2 + offset / 4, 1.0f, 1.0f, 0.0f, 1.0f, // donji levi
        centerX - 0.5f + smallIslandWidth / 2 - offset / 2+0.1, centerY - 0.5f - smallIslandHeight / 2 + offset / 4, 1.0f, 1.0f, 0.0f, 1.0f  // donji desni
    };

    float circle[(CRES + 2) * 2];
    float r = 0.1; //poluprecnik

    float aspectRatio = screenWidth / screenHeight;

    float centerXRed = static_cast<float>(xCircleCenter);
    float centerYRed = static_cast<float>(yCircleCenter);

    circle[0] = 0.0f; //Centar X0
    circle[1] = 0.0f; //Centar Y0
    int i;
    for (i = 0; i <= CRES; i++)
    {

        circle[2 + 2 * i] = centerXRed + r * cos((3.141592 / 180) * (i * 360 / CRES)); //Xi (Matematicke funkcije rade sa radijanima)
        circle[2 + 2 * i + 1] = centerYRed+ r* aspectRatio * sin((3.141592 / 180) * (i * 360 / CRES)); //Yi
    }



    float whiteCircle[(CRES + 2) * 2];
    float rWhite = 0.1; //poluprecnik

    float centerXWhite = 0.0f;
    float centerYWhite = 0.0f;

    whiteCircle[0] = 0.0f; //Centar X0
    whiteCircle[1] = 0.0f; //Centar Y0
    //int i;
    for (i = 0; i <= CRES; i++)
    {

        whiteCircle[2 + 2 * i] = centerXWhite + rWhite * cos((3.141592 / 180) * (i * 360 / CRES)); //Xi (Matematicke funkcije rade sa radijanima)
        whiteCircle[2 + 2 * i + 1] = centerYWhite + rWhite * aspectRatio * sin((3.141592 / 180) * (i * 360 / CRES)); //Yi
    }

    float fireVertices[] =
    {   //X    Y      S    T 
        -0.15f, -0.15f,   1.0f, 0.0f, 1.0f, 1.0f, 0.0f,//prvo tjeme
        -0.05f, -0.15f,   1.0f, 0.0f, 1.0f, 1.0f, 0.0f, //drugo tjeme
        -0.10f, 0.17-0.15f,     1.0f, 0.0f, 1.0f, 1.0f, 1.0f //trece tjeme
    };


    float palmVertices[] =
    {
        // X      Y       S    T 
        0.15,  -0.05,   1.0, 1.0,  // gornji desni
        0.15, -0.15,   1.0, 0.0,  // donji desni
       0.05, -0.15,   0.0, 0.0,  // donji levi
       0.05,  -0.05,   0.0, 1.0   // gornji levi
    };

    unsigned int palmIndices[] = {
        0, 1, 3,  // prvi trougao (gornji desni, donji desni, gornji levi)
        1, 2, 3   // drugi trougao (donji desni, donji levi, gornji levi)
    };

    float palmVertices1[] =
    {
        // X      Y       S    T 
        0.15,  0.15,   1.0, 1.0,  // gornji desni
        0.15, -0.17,   1.0, 0.0,  // donji desni
       0.05, -0.17,   0.0, 0.0,  // donji levi
       0.05,  0.15,   0.0, 1.0   // gornji levi
    };

    unsigned int palmIndices1[] = {
        0, 1, 3,  // prvi trougao (gornji desni, donji desni, gornji levi)
        1, 2, 3   // drugi trougao (donji desni, donji levi, gornji levi)
    };


    float sharkVertices[] =
    {
        //X       Y
        -0.5f,   -0.25f,   1.0f, 0.0f, 0.0f, 1.0f, // Prvo tjeme (levo)
        -0.3f,    -0.15f,   1.0f, 0.0f, 0.0f, 1.0f, // Drugo tjeme (desno)
        -0.4f,    -0.20f,     1.0f, 0.0f, 0.0f, 1.0f  // Treæe tjeme (vrh)
    };

    float newSharkVertices[] = {
        //// X       Y       R      G     B     A
        -0.7f,   -0.41f,    0.0f,  0.0f, 0.0f, 1.0f,  // Prvo tjeme (levo, crveno)
         -0.6f,   -0.41f,    0.0f,  0.0f, 0.0f, 1.0f,  // Drugo tjeme (desno, zeleno)
         -0.65f,    -0.21f,    0.0f,  0.0f, 0.0f, 1.0f,   // Treæe tjeme (gore, plavo)

       -0.9f, -0.45f,  0.0f, 0.0f, 0.0f, 1.0f,  // Levo dno
        -0.8f, -0.45f,  0.0f, 0.0f, 0.0f, 1.0f,  // Desno dno
        -0.85f,  -0.25f,  0.0f, 0.0f, 0.0f, 1.0f,   // Vrh

        0.5f,  -0.5f,  0.0f, 0.0f, 0.0f, 1.0f,  // Levo dno (crveno)
        0.6f,  -0.5f,  0.0f, 0.0f, 0.0f, 1.0f,  // Desno dno (zeleno)
        0.55f, -0.3f,  0.0f, 0.0f, 0.0f, 1.0f   // Vrh (plavo)
    };

    float clouds[] =
    {
        // X      Y      S    T
         0.6f,  0.4f,   0.0f, 0.0f,   // Donje levo
         0.7f,  0.4f,   1.0f, 0.0f,   // Donje desno
         0.6f,  0.6f,   0.0f, 1.0f,   // Gornje levo
         0.7f,  0.6f,   1.0f, 1.0f    // Gornje desno
    };
    
    float currentCloud = 3.0f;

    unsigned int cloudsIndices[] = {
        0, 1, 2,  // prvi trougao (gornji desni, donji desni, gornji levi)
        1, 3, 2   // drugi trougao (donji desni, donji levi, gornji levi)
    };


    float transparencyVertices[] = {
        // X       Y         S, T
        -0.20f, -0.15f,   0.0f,  0.0f, 0.0f,
         0.0f, -0.15f,   1.0f,  0.0f, 0.0f,
        -0.20f,  0.0f,   0.0f,  1.0f, 1.0f,
         0.0f,  0.0f,   1.0f,  1.0f, 1.0f
    };

    unsigned int transparencyIndices[] = {
    0, 1, 2,  
    1, 3, 2
    };

    float starPositions[] = {
    -0.8f, 0.5f,  // Zvezda 1
    0.3f, 0.7f,  // Zvezda 2
    -0.1f, 0.6f, // Zvezda 3
    0.6f, 0.8f, // Zvezda 4
    -0.7f, 0.6f  // Zvezda 5
    };

    float starSizes[] = {
    0.02f,  // Zvezda 1
    0.03f,  // Zvezda 2
    0.01f,  // Zvezda 3
    0.04f,  // Zvezda 4
    0.02f   // Zvezda 5
    };

    //unsigned int stride = (6) * sizeof(float);

    //unsigned int strideTexture = (2 + 2) * sizeof(float);

    //unsigned int VAO[100];
    glGenVertexArrays(100, VAO);     //cuva objekte kao nizove

    //unsigned int VBO[103];
    glGenBuffers(103, VBO);
    
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), skyVertices, GL_STATIC_DRAW);
   
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(seaVertices), seaVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(islandVertices), islandVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindVertexArray(VAO[3]);                  //circle
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindVertexArray(VAO[16]);                  //circle
    glBindBuffer(GL_ARRAY_BUFFER, VBO[16]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(whiteCircle), whiteCircle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindVertexArray(VAO[4]);                  //palm
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glGenBuffers(1, &EBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(palmVertices), palmVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(palmIndices), palmIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, strideTexture, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, strideTexture, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindVertexArray(VAO[5]);                  //fire
    glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fireVertices), fireVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(6 * sizeof(float)));  // Gornje teme
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindVertexArray(VAO[6]);                  //circle
    glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(redCircle), redCircle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindVertexArray(VAO[7]);                  //shark
    glBindBuffer(GL_ARRAY_BUFFER, VBO[7]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sharkVertices), sharkVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindVertexArray(VAO[8]);                  //clouds
    glBindBuffer(GL_ARRAY_BUFFER, VBO[8]);
    glGenBuffers(1, &EBOClouds);
    glBufferData(GL_ARRAY_BUFFER, sizeof(clouds), clouds, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOClouds);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cloudsIndices), cloudsIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, strideTexture, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, strideTexture, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    glBindVertexArray(VAO[17]); // Za vatru
    glBindBuffer(GL_ARRAY_BUFFER, VBO[17]);
    glGenBuffers(1, &EBOFireTrans);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparencyVertices), transparencyVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOFireTrans);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(transparencyIndices), transparencyIndices, GL_STATIC_DRAW);

    // Prvi atribut: Pozicija (X, Y)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, strideTextureFire, (void*)0);
    glEnableVertexAttribArray(0);

    // Drugi atribut: Koordinate teksture (S, T)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, strideTextureFire, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Treæi atribut: Da li je vrh ili nije (float)
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, strideTextureFire, (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Oèistimo
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Oèistimo
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindVertexArray(VAO[9]); // Bindujemo VAO za zvezde

    // Kreiramo VBO za pozicije zvezda
    glBindBuffer(GL_ARRAY_BUFFER, VBO[9]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(starPositions), starPositions, GL_STATIC_DRAW);
    // Postavljamo atribute za pozicije (location 0, 2 floats po taèki)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Kreiramo VBO za velièine zvezda
    glBindBuffer(GL_ARRAY_BUFFER, VBO[10]); // Koristimo sledeæi slobodan VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(starSizes), starSizes, GL_STATIC_DRAW);
    // Postavljamo atribute za velièine (location 1, 1 float po taèki)
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(VAO[11]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[11]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(newSharkVertices), newSharkVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Završavamo konfiguraciju
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenVertexArrays(1, &VAO[12]);
    glGenBuffers(1, &VBO[12]);
    glBindVertexArray(VAO[12]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[12]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindVertexArray(VAO[13]);                  //palm
    glBindBuffer(GL_ARRAY_BUFFER, VBO[13]);
    glGenBuffers(1, &EBOPalm);
    glBufferData(GL_ARRAY_BUFFER, sizeof(palmVertices1), palmVertices1, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOPalm);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(palmIndices1), palmIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, strideTexture, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, strideTexture, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenVertexArrays(1, &VAO[14]);
    glGenBuffers(1, &VBO[14]);
    glBindVertexArray(VAO[14]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[14]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    float positionSharks[] = {
        -0.25f, -0.5f,              //shark1
        -0.85f, -0.6f,
        -0.55f, -0.5f
    };

    float movingSharks[] = {
     0.0f, 0.0f,              //shark1
     0.0f, 0.0f,
     0.0f, 0.0f
    };

    unsigned int basicShader = createShader("basic.vert", "basic.frag");

    unsigned int basicShaderExtends = createShader("basic.vert", "basicExtend.frag");

    unsigned int basicShader11 = createShader("basic.vert", "basic11.frag");

    unsigned int basicShark = createShader("shark.vert", "basic.frag");

    unsigned int skyShader = createShader("basic.vert", "sky.frag");

    unsigned int palmShader = createShader("basic.vert", "fire.frag");

    unsigned int redShader = createShader("prim.vert", "redcircle.frag");

    unsigned int fireShader = createShader("basic.vert", "basic.frag");

    unsigned int extendShader = createShader("extendFire.vert", "extendFire.frag");

    unsigned int transparencyShader = createShader("fire.vert", "transparency.frag");

    unsigned int circleShader = createShader("moving.vert", "redcircle.frag");

    unsigned int cloudShader = createShader("fire.vert", "fire.frag");

    unsigned int transShader = createShader("transparency.vert", "transparency.frag");

    unsigned int starShader = createShader("star.vert", "star.frag");

    unsigned int sharkShader = createShader("shark.vert", "shark.frag");

    unsigned int sharkShaderExtend = createShader("sharkCircle.vert", "shark.frag");

    unsigned int extendCircleShader = createShader("extendCircle.vert", "extendCircle.frag");

    unsigned int textShader = createShader("text.vert", "text.frag");

    unsigned int palmShader1 = createShader("palm1.vert", "fire.frag");

    unsigned int extendCircleShader1 = createShader("fire.vert", "redCircle.frag");




    //unsigned int waterLevelLoc = glGetUniformLocation(basicShader, "waterLevel");
    //unsigned int skyHeightLoc = glGetUniformLocation(basicShader, "skyHeight");
    unsigned int seaHeightLoc = glGetUniformLocation(basicShader, "seaHeight");
    bool isIslandLoc = glGetUniformLocation(basicShader, "isIsland");
    bool isFireLoc = glGetUniformLocation(basicShader, "isFire");
    //bool transparencyLoc = glGetUniformLocation(basicShader, "isTransparency");

    unsigned int seaHeightLocExtend = glGetUniformLocation(basicShaderExtends, "seaHeight");
    bool isIslandLocExtend = glGetUniformLocation(basicShaderExtends, "isIsland");
    bool isFireLocExtend = glGetUniformLocation(basicShaderExtends, "isFire");
    //bool transparencyLoc = glGetUniformLocation(basicShaderExtends, "transparencySea");




    unsigned int seaHeightLoc11 = glGetUniformLocation(basicShader11, "seaHeight");
    bool isIslandLoc11 = glGetUniformLocation(basicShader11, "isIsland");
    bool isFireLoc11 = glGetUniformLocation(basicShader11, "isFire");

    unsigned int seaHeightLocShark1 = glGetUniformLocation(basicShark, "seaHeight");
    bool isIslandLocShark1 = glGetUniformLocation(basicShark, "isIsland");
    bool isFireLocShark1 = glGetUniformLocation(basicShark, "isFire");
    unsigned int moveXLocShark1 = glGetUniformLocation(basicShark, "moveX");
    unsigned int moveYLocShark1 = glGetUniformLocation(basicShark, "moveY");

    unsigned int seaHeightLoc1 = glGetUniformLocation(palmShader, "seaHeight");
    bool isIslandLoc1 = glGetUniformLocation(palmShader, "isIsland");
    bool isFireLoc1 = glGetUniformLocation(palmShader, "isFire");

    unsigned int seaHeightFireLoc = glGetUniformLocation(fireShader, "seaHeight");
    unsigned int extendFireLoc = glGetUniformLocation(extendShader, "extend");
    unsigned int circlePosLoc = glGetUniformLocation(circleShader, "uPos");

    unsigned int seaHeightLocFire = glGetUniformLocation(extendShader, "seaHeight");
    bool isIslandLocFire = glGetUniformLocation(extendShader, "isIsland");
    bool isFireLocFire = glGetUniformLocation(extendShader, "isFire");
    unsigned int flameCenterLoc = glGetUniformLocation(extendShader, "fireCenterUniformX");

    int flameIntensityLoc = glGetUniformLocation(extendShader, "flameIntensity");

    unsigned int seaHeightLocShark = glGetUniformLocation(sharkShader, "seaHeight");
    bool isIslandLocShark = glGetUniformLocation(sharkShader, "isIsland");
    bool isFireLocShark = glGetUniformLocation(sharkShader, "isFire");
    unsigned int moveXLoc = glGetUniformLocation(sharkShader, "moveX");
    unsigned int moveYLoc = glGetUniformLocation(sharkShader, "moveY");
    //bool isCircleLocShark = glGetUniformLocation(sharkShader, "isCircle");
    //bool isCircleLoc = glGetUniformLocation(sharkShader, "isCircle");


    unsigned int seaHeightLocTrans = glGetUniformLocation(transparencyShader, "seaHeight");
    bool isIslandLocTrans = glGetUniformLocation(transparencyShader, "isIsland");
    bool isFireLocTrans = glGetUniformLocation(transparencyShader, "isFire");
    unsigned int timeLocTrans = glGetUniformLocation(transparencyShader, "time");

    unsigned int seaHeightLocTrans1 = glGetUniformLocation(transShader, "seaHeight");
    bool isIslandLocTrans1 = glGetUniformLocation(transShader, "isIsland");
    bool isFireLocTrans1 = glGetUniformLocation(transShader, "isFire");
    unsigned int timeLocTrans1 = glGetUniformLocation(transShader, "time");
    unsigned int dynamicColorLocation1 = glGetUniformLocation(transShader, "dynamicColor");
    //unsigned int timeLocation1 = glGetUniformLocation(transShader, "time");



    unsigned int seaHeightLocSharkExtend = glGetUniformLocation(sharkShaderExtend, "seaHeight");
    bool isIslandLocSharkExtend = glGetUniformLocation(sharkShaderExtend, "isIsland");
    bool isFireLocSharkExtend = glGetUniformLocation(sharkShaderExtend, "isFire");
    unsigned int moveXLocExtend = glGetUniformLocation(sharkShaderExtend, "moveX");
    unsigned int moveYLocExtend = glGetUniformLocation(sharkShaderExtend, "moveY");
    unsigned int centerXLocExtend = glGetUniformLocation(sharkShaderExtend, "centerX");
    unsigned int centerYLocExtend = glGetUniformLocation(sharkShaderExtend, "centerY");
    unsigned int moveXXLocExtend = glGetUniformLocation(sharkShaderExtend, "moveXX");
    unsigned int moveYYLocExtend = glGetUniformLocation(sharkShaderExtend, "moveYY");


    unsigned int timeLocationFire = glGetUniformLocation(extendShader, "time");
    unsigned int fireExpansionLocFire = glGetUniformLocation(extendShader, "fireExpansion");
    int dynamicColorLocation = glGetUniformLocation(skyShader, "dynamicColor");

    unsigned int cloudShaderLoc = glGetUniformLocation(cloudShader, "uPos");

    unsigned int transShaderLoc = glGetUniformLocation(transShader, "uPos");


    //unsigned int palmShaderLoc1 = glGetUniformLocation(palmShader1, "uPos");
    unsigned int seaHeightLocPalm = glGetUniformLocation(palmShader1, "seaHeight");
    bool isIslandLocPalm = glGetUniformLocation(palmShader1, "isIsland");
    bool isFireLocPalm = glGetUniformLocation(palmShader1, "isFire");

    unsigned int timeLocExtend = glGetUniformLocation(extendCircleShader, "time");
    unsigned int durationLocExtend = glGetUniformLocation(extendCircleShader, "duration");
    unsigned int initialSizeLoc = glGetUniformLocation(extendCircleShader, "initialSize");
    unsigned int finalSizeLoc = glGetUniformLocation(extendCircleShader, "finalSize");
    unsigned int animationFactorLocation= glGetUniformLocation(extendCircleShader, "animationFactor");


    unsigned int extendCircleShader1Loc = glGetUniformLocation(extendCircleShader1, "uPos");


    unsigned checkerTexture = loadImageToTexture("res/palm.png");
    glBindTexture(GL_TEXTURE_2D, checkerTexture);
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(palmShader);
    unsigned uTexLoc = glGetUniformLocation(palmShader, "uTex");
    glUniform1i(uTexLoc, 0); // Indeks teksturne jedinice (sa koje teksture ce se citati boje)

    unsigned checkerTextureClouds = loadImageToTexture("res/clouds.png");
    glBindTexture(GL_TEXTURE_2D, checkerTextureClouds);
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(palmShader);

    unsigned checkerTextureTrans = loadImageToTexture("res/flame.png");
    glBindTexture(GL_TEXTURE_2D, checkerTextureTrans);
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(palmShader);

    unsigned checkerTexturePalm = loadImageToTexture("res/palm.png");
    glBindTexture(GL_TEXTURE_2D, checkerTexturePalm);
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(palmShader);
    //glUseProgram(0);
    //Odnosi se na glAct

    GLFWcursor* fishingHookCursor = createCustomCursor("res/rodee.png", 40, 40);
    if (fishingHookCursor) {
        glfwSetCursor(window, fishingHookCursor);
    }

    double time;

    float timeSharks= 0.0f;

    float currentWaterLevel;

    float x;

    float baseTime = 0.0f;

    float threshold = 0.3f;

    float directionCircleX = 0.0f;

    float directionCircleY = 0.0f;

    float directionCircleX1 = 0.0f;

    float directionCircleY1 = 0.0f;

    //static float posX = centerXR;
    glEnable(GL_PROGRAM_POINT_SIZE);

    std::chrono::duration<double, std::milli> sleep_duration(1000.0 / 60.0);

    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);              //UNIFORM ZA TEKST
    glUseProgram(textShader);
    int position = glGetUniformLocation(textShader, "position");
    glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    float directions[] = { 1.0f, 1.0f, -1.0f };

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int counter = 0;

    std::string textName = "Matija Popovic SV5/2021";

    float y;

    double timeMerenje = -4;

    int counterTransparencySea = 0;
    bool transparencySea = false;
    bool isBPress = false;

    float speed = 0.5;

    float lastPositionX = positionSharks[0];
    float lastPositionY = positionSharks[1];

    float lastPositionX1 = positionSharks[2];
    float lastPositionY1 = positionSharks[3];
    bool isLast1 = true;
    bool isLast2 = true;

    std::cout << lastPositionY << std::endl;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto startTimeFPS = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window))
    {
        auto startTimeFPS = std::chrono::high_resolution_clock::now();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        {
            double currentTimePress = glfwGetTime();
            if (currentTimePress - lastSpacePressTime >= 0.5) {
                lastSpacePressTime = currentTimePress;
                if (transparencySea) {
                    transparencySea = false;
                    //glEnable(GL_BLEND);
                    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                }
                else {
                    transparencySea = true;
                    //glDisable(GL_BLEND);
                }
            }
            //transparencySea = true;
        }
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            std::cout << "UDJE U R";
            speed = 0.5;
            //movingSharks[0] = lastPositionX;
            //movingSharks[1] = lastPositionY;
            //movingSharks[2] = lastPositionX1;
            //movingSharks[3] = lastPositionY1;
            double currentTimePress = glfwGetTime();
            if (currentTimePress - lastSpacePressTimeR >= 0.5) {
                lastSpacePressTimeR = currentTimePress;
                lastPositionX = movingSharks[0];
                lastPositionY = movingSharks[1];
                lastPositionX1 = movingSharks[2];
                lastPositionY1 = movingSharks[3];
            }
            //lastPositionX = movingSharks[0];
            //lastPositionY = movingSharks[1];
        }

        if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
        {
            std::cout << "UDJE U +";
            speed = 1.0;
            double currentTimePress = glfwGetTime();
            if (currentTimePress - lastSpacePressTimeP >= 0.5) {
                lastSpacePressTimeP = currentTimePress;
                lastPositionX = movingSharks[0];
                lastPositionY = movingSharks[1];
                lastPositionX1 = movingSharks[2];
                lastPositionY1 = movingSharks[3];
            }
        }

        if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
        {
            std::cout << "UDJE U -" << std::endl;
            speed = 0.3;
            double currentTimePress = glfwGetTime();
            if (currentTimePress - lastSpacePressTimeM >= 0.5) {
                lastSpacePressTimeM = currentTimePress;
                lastPositionX = movingSharks[0];
                lastPositionY = movingSharks[1];
                lastPositionX1 = movingSharks[2];
                lastPositionY1 = movingSharks[3];
            }
        }

        counter++;
        if (counter % trashold_frame==0) {
            global_counter++;
            if (global_counter < 5) {
                current_text += textPOMOC[4-global_counter];
            }
        }
        
        time = glfwGetTime();

        timeSharks = glfwGetTime();
        
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        glUseProgram(basicShader);

        currentWaterLevel = abs(sin(glfwGetTime()) * 0.2f);  // Dinamièka promena nivoa vode
        glUniform1f(seaHeightLoc, currentWaterLevel);
        glUniform1f(isIslandLoc, false);
        glUniform1f(isFireLoc, false);
        //glUniform1f(transparencyLoc, false);
       
       // glClearColor(0.5, 0.5, 0.5, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(skyShader);

        glBindVertexArray(VAO[0]);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        float intensity = (sin(speed* time * 1.2f+2) + 1.0f) / 2.0f; // Vrednost izmeðu 0.0 i 1.0
        float r = 0.05f + intensity * 0.15f; // Interpolacija crvene
        float g = 0.05f + intensity * 0.25f; // Interpolacija zelene
        float b = 0.2f + intensity * 0.3f;  // Interpolacija plave

        glUniform3f(dynamicColorLocation, r, g, b);

        glBindVertexArray(VAO[1]);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        //glBindVertexArray(VAO[0]);

        //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glDisable(GL_BLEND);

        glUseProgram(circleShader);
        glUniform2f(circlePosLoc, 1.2 * sin(time * speed + 2), 0.7 * cos(time * speed + 2));
        glUniform4f(glGetUniformLocation(circleShader, "inColVec"), 1.0, 1.0, 0.0, 0.0);

        glBindVertexArray(VAO[3]);

        glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(circle) / (2 * sizeof(float)));

        glUseProgram(circleShader);
        glUniform2f(circlePosLoc, 1.5 * sin(time * speed - 2), 0.7 * cos(time * speed - 2));
        glUniform4f(glGetUniformLocation(circleShader, "inColVec"), 1.0, 1.0, 1.0, 0.0);

        glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(circle) / (2 * sizeof(float)));

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (!transparencySea) {
            glUseProgram(basicShader);
            glUniform1f(seaHeightLoc, currentWaterLevel);
            glUniform1f(isIslandLoc, false);
            glUniform1f(isFireLoc, false);
        }
        else {
            glUseProgram(basicShader11);
            glUniform1f(seaHeightLoc11, currentWaterLevel);
            glUniform1f(isIslandLoc11, false);
            glUniform1f(isFireLoc11, false);
        }


        glBindVertexArray(VAO[1]);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        if (activateRedCircle) {
            isLast1 = true;
            std::cout << "X KRUG: " << redCircle[0] << " Y KRUG: " << redCircle[1] << std::endl;
            std::cout << lastPositionY << std::endl;

            if (isFirstUpdate) {
                startTime = glfwGetTime(); // Postavite poèetno vreme
                isFirstUpdate = false;
            }

            float elapsedTime = glfwGetTime() - startTime;
            float duration = 4.0f;

            if (elapsedTime > duration) {
                activateRedCircle = false;
            }
            else {
                rr += 0.0005f;;

                for (int i = 0; i <= CRES; i++) {
                    redCircle[2 + 2 * i] = redCircle[0] + rr * cosf((3.141592f / 180.0f) * (i * 360 / CRES)); // Xi
                    redCircle[2 + 2 * i + 1] = redCircle[1] + rr * aspectRatio * sinf((3.141592f / 180.0f) * (i * 360 / CRES)); // Yi
                }

                glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(redCircle), redCircle);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }

            glUseProgram(extendCircleShader);
            glUniform1f(timeLocExtend, glfwGetTime());
            glUniform1f(durationLocExtend, 4.0f);
            glUniform1f(initialSizeLoc, 0.2f); // Poèetna velièina
            glUniform1f(finalSizeLoc, 0.4f);   // Krajnja velièina

            glUniform1f(animationFactorLocation, 0.1f);

            glBindVertexArray(VAO[6]);
            glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(redCircle) / (2 * sizeof(float)));

            glUseProgram(basicShark);
            glUniform1f(seaHeightLocShark1, currentWaterLevel);
            glUniform1f(isIslandLocShark1, true);
            glUniform1f(isFireLocShark1, true);
            glUniform1f(moveXLocShark1, movingSharks[0]);
            glUniform1f(moveYLocShark1, movingSharks[1]);

            if (isLast2) {
                lastPositionX = movingSharks[0];
                lastPositionY = movingSharks[1];
                lastPositionX1 = movingSharks[2];
                lastPositionY1 = movingSharks[3];
                isLast2 = false;
                baseTime = glfwGetTime();
                std::cout << "UDJE 2" << std::endl;
                std::cout << lastPositionY << std::endl;
            }

            if (lastPositionX < redCircle[0]) {
                directionCircleX = 1;
            }
            else {
                directionCircleX = -1;
            }

            if (lastPositionY + positionSharks[1] < redCircle[1]) {
                directionCircleY = 1;
            }
            else {
                directionCircleY = -1;
            }

            if (lastPositionX1 < redCircle[0]) {
                directionCircleX1 = 1;
            }
            else {
                directionCircleX1 = -1;
            }

            if (lastPositionY1 + positionSharks[3] < redCircle[1]) {
                directionCircleY1 = 1;
            }
            else {
                directionCircleY1 = -1;
            }

            timeSharks = glfwGetTime() - baseTime;

            movingSharks[0] = lastPositionX + directionCircleX * speed * timeSharks * 0.05f;
            movingSharks[1] = lastPositionY + directionCircleY * speed * timeSharks * 0.05f;


            movingSharks[2] = lastPositionX1 + directionCircleX1 * speed * timeSharks * 0.05f;
            movingSharks[3] = lastPositionY1 + directionCircleY1 * speed * timeSharks * 0.05f;

            glBindVertexArray(VAO[11]);

            glDrawArrays(GL_TRIANGLES, 0, 3);

            glUniform1f(moveXLocShark1, movingSharks[2]);
            glUniform1f(moveYLocShark1, movingSharks[3]);

            glDrawArrays(GL_TRIANGLES, 3, 3);
        }
        else {
            rr = 0.05f;
            isLast2 = true;
            glUseProgram(basicShark);
            glUniform1f(seaHeightLocShark1, currentWaterLevel);
            glUniform1f(isIslandLocShark1, true);
            glUniform1f(isFireLocShark1, true);
            //glUniform1f(moveXLocShark1, directions[0]*speed * time * 0.05f);
            glUniform1f(moveXLocShark1, movingSharks[0]);
            glUniform1f(moveYLocShark1, movingSharks[1]);

            //lastPositionX = movingSharks[0];
            if (isLast1) {
                lastPositionX = movingSharks[0];
                lastPositionY = movingSharks[1];
                lastPositionX1 = movingSharks[2];
                lastPositionY1 = movingSharks[3];
                isLast1 = false;
                baseTime = glfwGetTime();
                //std::cout << "UDJE 1" << std::endl;
            }

            timeSharks = glfwGetTime() - baseTime;

            movingSharks[0] = lastPositionX + directions[0] * speed * timeSharks * 0.05f;
            movingSharks[1] = lastPositionY;

            movingSharks[2] = lastPositionX1 + directions[1] * speed * timeSharks * 0.05f;
            movingSharks[3] = lastPositionY1;

            glBindVertexArray(VAO[11]);

            glDrawArrays(GL_TRIANGLES, 0, 3);

            glUniform1f(moveXLocShark1, movingSharks[2]);
            glUniform1f(moveYLocShark1, movingSharks[3]);

            glDrawArrays(GL_TRIANGLES, 3, 3);
        }


        glUseProgram(basicShader);
        glUniform1f(isIslandLoc, true);
        glUniform1f(isFireLoc, true);


        glBindVertexArray(VAO[2]);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

        glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);


        glUseProgram(transShader);                    //OVDE JE CLOUD

        //glUniform2f(transShaderLoc, 0.0f, 0.0f);
        glUniform1f(seaHeightLocTrans1, currentWaterLevel);
        glUniform1i(isIslandLocTrans1, true);
        glUniform1i(isFireLocTrans1, true);
        //glUniform2f(glGetUniformLocation(transShader, "flameCenter"), 0.5f, 0.5f); // Centar plamena u sredini
        //glUniform1f(glGetUniformLocation(transShader, "flameRadius"), 0.25f); // Radijus plamena
        //glUniform1f(glGetUniformLocation(transShader, "flameIntensity"), 1.0f); // Intenzitet svetla
        glUniform1f(timeLocTrans1, glfwGetTime());

        std::cout << "Dynamic Color RGB: (" << r << ", " << g << ", " << b << ")" << std::endl;


        glUniform3f(dynamicColorLocation1, r, g, b);

        glBindVertexArray(VAO[17]);

        glActiveTexture(GL_TEXTURE0); // Aktiviraj teksturnu jedinicu 0
        glBindTexture(GL_TEXTURE_2D, checkerTextureTrans); // Binduj teksturu za SAMPLER2D uniformu

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindTexture(GL_TEXTURE_2D, 0);

        glUseProgram(extendShader);
        glUniform1f(seaHeightLocFire, currentWaterLevel);
        glUniform1i(isIslandLocFire, true);
        glUniform1i(isFireLocFire, true);
        glUniform1f(flameCenterLoc, 0.0f);
        glUniform1f(timeLocationFire, glfwGetTime());

        glBindVertexArray(VAO[5]);

        glDrawArrays(GL_TRIANGLES, 0, 3);


        // Renderovanje kvadrata koristeæi EBO
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // 6 jer imamo 6 indeksa za 2 trougla


        // Resetuj bindove (opciono, za èistoæu koda)
        glBindTexture(GL_TEXTURE_2D, 0);

        if (currentCloud < -1.0f) {
            currentCloud = 3.0f;
        }
        else
        {
            currentCloud -= 0.01f;
        }

        glUseProgram(cloudShader);                    //OVDE JE CLOUD

        glUniform2f(cloudShaderLoc, currentCloud , 0.0f);

        glBindVertexArray(VAO[8]);

        glActiveTexture(GL_TEXTURE0); // Aktiviraj teksturnu jedinicu 0
        glBindTexture(GL_TEXTURE_2D, checkerTextureClouds); // Binduj teksturu za SAMPLER2D uniformu

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindTexture(GL_TEXTURE_2D, 0);


        glUseProgram(starShader);

        glUniform1f(glGetUniformLocation(starShader, "time"), time);


        if (r < threshold && g < threshold && b < threshold) {
            glUseProgram(starShader);
            glBindVertexArray(VAO[9]);
            glDrawArrays(GL_POINTS, 0, 5); // 5 zvezda
            glBindVertexArray(0);
        }

        glUseProgram(palmShader1);                    //OVDE JE CLOUD

        glUniform1f(seaHeightLocPalm, currentWaterLevel);
        glUniform1i(isIslandLocPalm, true);
        glUniform1i(isFireLocPalm, true);

        glBindVertexArray(VAO[13]);

        glActiveTexture(GL_TEXTURE0); // Aktiviraj teksturnu jedinicu 0
        glBindTexture(GL_TEXTURE_2D, checkerTexturePalm); // Binduj teksturu za SAMPLER2D uniformu

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindTexture(GL_TEXTURE_2D, 0);


        glUseProgram(textShader);
        glUniform3f(glGetUniformLocation(textShader, "textColor"), 0.5f, 0.5f, 0.5f);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO[12]);

        //float y_offset = 0.0f; // Poèetni offset
        float move_speed = 1.0f; // Brzina kretanja
        float max_height = 350.0f; // Polovina neba

        if (activateFire) {
            float scale = 1.0f;
            float x = 342.0f;
            if (counter_strings == 0) {
                y = 312.0f + y_offset;
            }
            else if (counter_strings == 1) {
                y = 420.0f + y_offset;
            }
            else if (counter_strings == 2) {
                y = 450.0f + y_offset - 30.0f;
            }
            else if (counter_strings == 3) {
                y = 450.0f + y_offset - 50.0f;
            }
            else if (counter_strings == 4) {
                y = 450.0f + y_offset - 75.0f;
            }
            else {
                y = 450.0f;
            }

            for (char c : current_text) {                   //izmeniti ovo u text
                Character ch = Characters[c];

                float xpos = x + ch.Bearing.x * scale;
                float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

                float w = ch.Size.x * scale;
                float h = ch.Size.y * scale;

                // VBO za svaki karakter
                float vertices[6][4] = {
                    { xpos,     ypos + h,   0.0f, 0.0f },
                    { xpos,     ypos,       0.0f, 1.0f },
                    { xpos + w, ypos,       1.0f, 1.0f },

                    { xpos,     ypos + h,   0.0f, 0.0f },
                    { xpos + w, ypos,       1.0f, 1.0f },
                    { xpos + w, ypos + h,   1.0f, 0.0f }
                };

                glBindTexture(GL_TEXTURE_2D, ch.TextureID);
                glBindBuffer(GL_ARRAY_BUFFER, VBO[12]);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glDrawArrays(GL_TRIANGLES, 0, 6);

                y -= (ch.Advance >> 6) * scale; // Vertikalno pomeranje za svaki karakter
            }

            glBindTexture(GL_TEXTURE_2D, 0);
            glBindVertexArray(0);
            glUseProgram(0);

            y_offset += move_speed; // Pomeranje ka nebu

            // Resetovanje kada stigne do max_height
            if (y >= max_height) {
                counter++;       // Preði na sledeæe stanje

                if (!current_text.empty()) {
                    current_text.erase(0, 1); // Ukloni prvi karakter
                    //y_offset -= 50.0f;
                }

                //current_text[counter_strings] = ' ';
                // Resetuj y_offset i y, ali zadrži ravnomeran pomak za sledeæi ciklus
                y_offset = 0.0f;
                counter_strings++;
            }

            glBindTexture(GL_TEXTURE_2D, 0);
        }
        glBindVertexArray(0);
        glUseProgram(0);


        if (current_text.empty()) {
            activateFire = false;
        }
        
        glUseProgram(textShader);
        glUniform3f(glGetUniformLocation(textShader, "textColor"), 1.0f, 0.0f, 0.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO[14]);

        float scale = 0.25f;
        float x_name = 10.0f;
        float y_name = 50.0f;

        std::string text_target = "MATIJA POPOVIC SV5/2021";

        for (char c : text_target) {
            Character ch = Characters[c];

            float xpos = x_name + ch.Bearing.x * scale;
            float ypos = y_name - (ch.Size.y - ch.Bearing.y) * scale;
            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;

            // VBO za svaki karakter
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, VBO[14]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            x_name += (ch.Advance >> 6) * scale; // Vertikalno pomeranje za svaki karakter
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glUseProgram(0);


        glfwSwapBuffers(window);
        glfwPollEvents();
        //std::this_thread::sleep_for(sleep_duration);
        auto endTimeFPS = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsedFPS = endTimeFPS - startTimeFPS;

        double sleepTime = FRAME_TIME - elapsedFPS.count();
        if (sleepTime > 0) {
            std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
        }
    }

    if (fishingHookCursor) {
        glfwDestroyCursor(fishingHookCursor);
    }

    glDeleteProgram(extendCircleShader);
    glDeleteTextures(1, &checkerTexture);
    glDeleteTextures(1, &checkerTextureClouds);
    glDeleteTextures(1, &checkerTextureTrans);
    glDeleteBuffers(15, VBO);
    glDeleteVertexArrays(12, VAO);
    glDeleteProgram(redShader);
    glDeleteProgram(cloudShader);
    glDeleteProgram(starShader);
    glDeleteProgram(sharkShader);
    glDeleteProgram(textShader);
    glDeleteProgram(palmShader);
    glDeleteProgram(basicShader);


    glfwTerminate();
    return 0;
}

unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
     std::string temp = ss.str();
     const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);
    
    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }


    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}
static unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        //Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        // Provjerava koji je format boja ucitane slike
        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 2: InternalFormat = GL_RG; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        // oslobadjanje memorije zauzete sa stbi_load posto vise nije potrebna
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}


static void mouse_callback(GLFWwindow* window, double x, double y)
{
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mousePressed = true;
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        float ax = -0.15f, ay = -0.15f;
        float bx = -0.05f, by = -0.15f;
        float cx = -0.10f, cy = 0.02f;

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        float dim1 = xpos / width * 2.0f - 1.0f;
        float dim2 = 1.0f - ypos / height * 2.0f;

        //cursor 0,0 gornji levi ugao
        //sirina 800 moja visina 500
        //else if (dim1 < -0.04f && dim1 > -0.16f && dim2> -0.2f && dim2 <0.1f) {
        if (isPointInTriangle(dim1, dim2, ax, ay, bx, by, cx, cy)) {

                mouseInPosition = true;

                initializeHelp();

                std::cout << "Mouse clicked at X: " << dim1 << " Y: " << dim2 << std::endl;

        }
        else if(dim2<0.0f){
            std::cout << "USAO KRUG";
            mouseInPosition = true;

            initializeRedCircle(dim1, dim2);

            std::cout << "Mouse clicked at X: " << dim1 << " Y: " << dim2 << std::endl;
        }
        else {
            mouseInPosition = false;
        }
    }

}

void initializeRedCircle(double xCircleCenter, double yCircleCenter) {
    isFirstUpdate = true;
    activateRedCircle = true;
    float centerXR = xCircleCenter; // Skalirani centar
    float centerYR = yCircleCenter;

    std::cout << "X KRUG: " << centerXR << " Y KRUG: " << centerYR << std::endl;


    redCircle[0] = centerXR; // Centar X
    redCircle[1] = centerYR; // Centar Y

    // Raèunanje taèaka na krugu
    for (int i = 0; i <= CRES; i++) {
        redCircle[2 + 2 * i] = centerXR + rr * cosf((3.141592f / 180.0f) * (i * 360 / CRES)); // Xi
        redCircle[2 + 2 * i + 1] = centerYR + rr * aspectRatio * sinf((3.141592f / 180.0f) * (i * 360 / CRES)); // Yi
    }

    std::cout << "X KRUG: " << redCircle[0] << " Y KRUG: " << redCircle[1] << std::endl;


    // Ažuriraj VBO sa novim podacima
    glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(redCircle), redCircle); // Ažuriraj podatke u VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Oslobodi buffer
}

void initializeHelp() {
    std::cout << "UDJE HELP" << std::endl;
    activateFire = true;
    trashold_frame = 50;
    y_offset = 0.0f; // Poèetni offset
    textPOMOC = "COMOP";
    global_counter = 0;
    counterr = 0;
    current_text = "P";
    counter_strings = 0;
}


bool isPointInTriangle(float px, float py,
    float ax, float ay,
    float bx, float by,
    float cx, float cy) {
    float cross1 = (px - ax) * (by - ay) - (py - ay) * (bx - ax);
    float cross2 = (px - bx) * (cy - by) - (py - by) * (cx - bx);
    float cross3 = (px - cx) * (ay - cy) - (py - cy) * (ax - cx);
    return (cross1 >= 0 && cross2 >= 0 && cross3 >= 0) || (cross1 <= 0 && cross2 <= 0 && cross3 <= 0);
}

GLFWcursor* createCustomCursor(const char* imagePath, int desiredWidth, int desiredHeight) {
    int width, height, channels;
    unsigned char* data = loadImage(imagePath, width, height, channels);
    if (!data) {
        std::cerr << "Failed to load cursor image: " << imagePath << std::endl;
        return nullptr;
    }

    // Resize the image to the desired dimensions
    unsigned char* resizedData = resizeImage(data, width, height, channels, desiredWidth, desiredHeight);
    stbi_image_free(data); // Free the original image data

    GLFWimage image;
    image.width = desiredWidth;
    image.height = desiredHeight;
    image.pixels = resizedData;

    GLFWcursor* cursor = glfwCreateCursor(&image, 0, 0); // Set the hotspot to the top-left corner
    delete[] resizedData;
    return cursor;
}

unsigned char* loadImage(const char* path, int& width, int& height, int& channels) {
    return stbi_load(path, &width, &height, &channels, 0);
}

unsigned char* resizeImage(unsigned char* inputImage, int inputWidth, int inputHeight, int channels, int outputWidth, int outputHeight) {
    unsigned char* outputImage = new unsigned char[outputWidth * outputHeight * channels];
    if (stbir_resize_uint8(inputImage, inputWidth, inputHeight, 0, outputImage, outputWidth, outputHeight, 0, channels) != 1) {
        std::cerr << "Failed to resize image" << std::endl;
        delete[] outputImage;
        return nullptr;
    }
    return outputImage;
}

void initScreenDimensions() {
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);

    screenWidth = videoMode->width;
    screenHeight = videoMode->height;

    std::cout << "Screen Dimensions: " << screenWidth << "x" << screenHeight << std::endl;
}
