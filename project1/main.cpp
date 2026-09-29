/**
* Author: Xufei (Felix) Zhang
* Assignment: Draw a Simple 2D Scene
* Date due: [10/03/2026]
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/cs3113.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 550,
              SCREEN_HEIGHT = 550,
              FPS           = 60;

constexpr char BG_COLOUR[] = "#000000";

// Textures
constexpr char ONE_FP[]   = "assets/earth.png";
constexpr char TWO_FP[]   = "assets/water.png";
constexpr char THREE_FP[] = "assets/air.png";
constexpr char FOUR_FP[]  = "assets/fire.png";

// Global Variables
AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f;

float gCycler = 0.0f;
float gCyclerRad;

Vector2 gOnePosition = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
Vector2 gTwoPosition;
Vector2 gThreePosition;
Vector2 gFourPosition;

float gOneSpeed = 13.0f;
float gTwoSpeed = -19.0f;
float gThreeSpeed = 23.0f;
float gFourSpeed = 23.0f;

float gTwoDist = 100.0f;
float gThreeDist = 120.0f;
float gFourDist = 80.0f;

float gBaseSize = 20.0f;
float gOneSize = gBaseSize;
float gTwoSize = gBaseSize;
float gThreeSize = gBaseSize;
float gFourSize = gBaseSize;

float gPulseSpeed = 80.0f;
float gPulseOffset = 5.0f * DEG2RAD;
float gPulseRange = 5.0f;

Texture2D gOneTexture;
Texture2D gTwoTexture;
Texture2D gThreeTexture;
Texture2D gFourTexture;

Color gBgColour;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
void drawObject(Texture2D texture, Vector2 position, float size, float rotation);

void initialise() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Project 1");

    int monitor = GetCurrentMonitor();
    SetWindowPosition(
        (GetMonitorWidth(monitor)  - SCREEN_WIDTH)  / 2,
        (GetMonitorHeight(monitor) - SCREEN_HEIGHT) / 2
    );

    gOneTexture   = LoadTexture(ONE_FP);
    gTwoTexture   = LoadTexture(TWO_FP);
    gThreeTexture = LoadTexture(THREE_FP);
    gFourTexture  = LoadTexture(FOUR_FP);

    SetTargetFPS(FPS);
}

void processInput() {
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() {
    float ticks = GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;
    gCycler = fmod(gCycler + (1.0f * deltaTime), 360.0f);  
    gCyclerRad = gCycler * DEG2RAD;

    gTwoPosition = {
        gOnePosition.x + (gTwoDist * cosf(gCyclerRad * gOneSpeed)),
        gOnePosition.y + (gTwoDist * sinf(gCyclerRad * gOneSpeed))
    };

    float twoRotationRad = gCycler * gTwoSpeed * DEG2RAD;
    gThreePosition = {
        gTwoPosition.x + (gThreeDist * cosf(gCyclerRad * gTwoSpeed)),
        gTwoPosition.y + (gThreeDist * sinf(gCyclerRad * gTwoSpeed))
    };

    float threeRotationRad = gCycler * gThreeSpeed * DEG2RAD;
    gFourPosition = {
        gThreePosition.x + (gFourDist * cosf(gCyclerRad * gThreeSpeed)),
        gThreePosition.y + (gFourDist * sinf(gCyclerRad * gThreeSpeed))
    };

    gOneSize = sinf((gCyclerRad)*gPulseSpeed)*gPulseRange
        +gBaseSize;
    gTwoSize = sinf((gCyclerRad-gPulseOffset)*gPulseSpeed)*gPulseRange
        +gBaseSize;
    gThreeSize = sinf((gCyclerRad-(2*gPulseOffset))*gPulseSpeed)*gPulseRange
        +gBaseSize;
    gFourSize = sinf((gCyclerRad-(3*gPulseOffset))*gPulseSpeed)*gPulseRange
        +gBaseSize;

    Vector2 delta = { 
        gFourPosition.x - gOnePosition.x, 
        gFourPosition.y - gOnePosition.y 
    };
    float bgHue = (atan2f(delta.y, delta.x) + PI) * RAD2DEG;
    gBgColour = ColorFromHSV(bgHue, 0.5f, 0.2f);
}

void drawObject(Texture2D texture, Vector2 position, 
    float size, float rotation) {
    Rectangle sourceArea = {
        0.0f, 0.0f,
        static_cast<float>(texture.width),
        static_cast<float>(texture.height)
    };

    Rectangle destinationArea = {
        position.x, position.y,
        size * 2.0f, size * 2.0f
    };

    Vector2 originOffset = {size, size};

    DrawTexturePro(texture, sourceArea, destinationArea, 
        originOffset, rotation, WHITE);
}

void render() {
    BeginDrawing();

    ClearBackground(gBgColour);

    // One
    drawObject(gOneTexture, gOnePosition, gOneSize, gCycler * gOneSpeed);

    // Two
    DrawLineEx(gOnePosition, gTwoPosition, 2.0f, WHITE);
    drawObject(gTwoTexture, gTwoPosition, gTwoSize, gCycler * gTwoSpeed);

    // Three
    DrawLineEx(gTwoPosition, gThreePosition, 2.0f, WHITE);
    drawObject(gThreeTexture, gThreePosition, gThreeSize, gCycler * gThreeSpeed);

    // Four
    DrawLineEx(gThreePosition, gFourPosition, 2.0f, WHITE);
    drawObject(gFourTexture, gFourPosition, gFourSize, gCycler * gFourSpeed);

    EndDrawing();
}

void shutdown() {
    UnloadTexture(gOneTexture);
    UnloadTexture(gTwoTexture);
    UnloadTexture(gThreeTexture);
    UnloadTexture(gFourTexture);

    CloseWindow();
}

int main(void) {
    initialise();

    while (gAppStatus == RUNNING) {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}