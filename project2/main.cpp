/**
* Author: Xufei (Felix) Zhang
* Assignment: Pong Clone
* Date due: [10/16/2026]
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/cs3113.h"

enum GameStatus {PLAYING, GAME_OVER};

// Global Constants
constexpr int SCREEN_WIDTH = 1000;
constexpr int SCREEN_HEIGHT = 600; 
constexpr int FPS = 60;

constexpr int MAX_BALLS = 5;
constexpr int WIN_SCORE = 5;

constexpr float PADDLE_SPEED = 400.0f;
constexpr float AI_SPEED = 300.0f;
constexpr float BALL_SPEED = 350.0f;

constexpr Vector2 ORIGIN = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
constexpr Vector2 P1_START_POS = {50.0f, ORIGIN.y};
constexpr Vector2 P2_START_POS = {SCREEN_WIDTH - 50.0f, ORIGIN.y};

constexpr Vector2 PADDLE_SCALE = {32.0f, 128.0f};
constexpr Vector2 BALL_SCALE = {32.0f, 32.0f};
constexpr Vector2 DIGIT_SCALE = {33.0f, 45.0f}; 
constexpr Vector2 STAR_SCALE = {90.0f, 90.0f};

constexpr char PADDLE_PATH[] = "assets/paddle.png";
constexpr char PADDLE_BOLT_PATH[] = "assets/paddle_bolt.png";
constexpr char BALL_PATH[] = "assets/ball.png";
constexpr char STAR_PATH[] = "assets/star.png";
constexpr char DIGIT_PATHS[10][20] = {
  "assets/number_0.png", "assets/number_1.png", 
  "assets/number_2.png", "assets/number_3.png", 
  "assets/number_4.png", "assets/number_5.png",
  "assets/number_6.png", "assets/number_7.png", 
  "assets/number_8.png", "assets/number_9.png"
}; 
  
constexpr char BG_COLOR[] = "#19192a";
constexpr char P1_COLOR[] = "#43b15d";
constexpr char P2_COLOR[] = "#e1bb3e";

// Global Variables
AppStatus  gAppStatus = RUNNING;
GameStatus gGameStatus = PLAYING;
float gPreviousTicks = 0.0f;

Vector2 gP1Position = P1_START_POS;
Vector2 gP1Movement = {0.0f, 0.0f};
Vector2 gP2Position = P2_START_POS;
Vector2 gP2Movement = {0.0f, 0.0f};

bool gIsSinglePlayer = false;
float gAIDirection = 1.0f;

// Only the first gActiveBalls are updated and renderd
Vector2 gBallPositions[MAX_BALLS];
Vector2 gBallMovements[MAX_BALLS];
int gActiveBalls = 1;

int gP1Score = 0;
int gP2Score = 0;

Texture2D gPaddleTexture;
Texture2D gPaddleBoltTexture;
Texture2D gBallTexture;
Texture2D gStarTexture;
Texture2D gDigitTextures[10];

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
bool isColliding(const Vector2* positionA, const Vector2* scaleA,
                 const Vector2* positionB, const Vector2* scaleB);
void renderObject(const Texture2D* texture, const Vector2* position,
                  const Vector2* scale, Color tint);
void resetBall(int index, float xDirection);

// Function Definitions
bool isColliding(const Vector2* positionA, const Vector2* scaleA,
                 const Vector2* positionB, const Vector2* scaleB) {
  float xDistance = fabs(positionA->x - positionB->x) - 
    ((scaleA->x + scaleB->x) / 2.0f);
  float yDistance = fabs(positionA->y - positionB->y) - 
    ((scaleA->y + scaleB->y) / 2.0f);
  
  return xDistance < 0.0f && yDistance < 0.0f;
}

void renderObject(const Texture2D* texture, const Vector2* position,
                  const Vector2* scale, Color tint) {
  Rectangle textureArea = {
    0.0f, 0.0f,
    static_cast<float>(texture->width),
    static_cast<float>(texture->height)
  };

  Rectangle destinationArea = {
    position->x, position->y,
    scale->x, scale->y
  };

  Vector2 originOffset = {scale->x / 2.0f, scale->y / 2.0f};
  DrawTexturePro(*texture, textureArea, destinationArea, 
    originOffset, 0.0f, tint);
}

void resetBall(int index, float xDirection) {
  gBallPositions[index] = {ORIGIN.x, ORIGIN.y + 
    (index - MAX_BALLS / 2) * 50.0f};
  gBallMovements[index] = {xDirection, (index % 2 == 0) ? 0.5f : -0.5f};
  Normalise(&gBallMovements[index]);
}

void initialise() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Project 2");

  gPaddleTexture = LoadTexture(PADDLE_PATH);
  gPaddleBoltTexture = LoadTexture(PADDLE_BOLT_PATH); 
  gBallTexture = LoadTexture(BALL_PATH); 
  gStarTexture = LoadTexture(STAR_PATH);

  for (int i = 0; i < 10; i++) gDigitTextures[i] = LoadTexture(DIGIT_PATHS[i]);

  for (int i = 0; i < MAX_BALLS; i++) resetBall(i, (i % 2 == 0) ? 1.0f : -1.0f);

  SetTargetFPS(FPS);
}

void processInput() {
  if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;

  if (gGameStatus == GAME_OVER) return;

  // Singleplayer?
  if (IsKeyPressed(KEY_T)) gIsSinglePlayer = !gIsSinglePlayer; 

  // Player 1
  gP1Movement = {0.0f, 0.0f};
  if (IsKeyDown(KEY_W)) gP1Movement.y = -1.0f;
  else if (IsKeyDown(KEY_S)) gP1Movement.y = 1.0f;

  // Player 2
  gP2Movement = {0.0f, 0.0f};
  if (!gIsSinglePlayer) {
    if (IsKeyDown(KEY_UP)) gP2Movement.y = -1.0f;
    else if (IsKeyDown(KEY_DOWN)) gP2Movement.y = 1.0f;
  }

  // Number of balls
  if (IsKeyPressed(KEY_ONE)) gActiveBalls = 1;
  if (IsKeyPressed(KEY_TWO)) gActiveBalls = 2; 
  if (IsKeyPressed(KEY_THREE)) gActiveBalls = 3; 
  if (IsKeyPressed(KEY_FOUR)) gActiveBalls = 4; 
  if (IsKeyPressed(KEY_FIVE)) gActiveBalls = 5; 
}

void update() {
  float ticks = static_cast<float>(GetTime());
  float deltaTime = ticks - gPreviousTicks;
  gPreviousTicks = ticks;

  if (gGameStatus == GAME_OVER) return;

  // Paddles
  gP1Position.y += PADDLE_SPEED * gP1Movement.y * deltaTime;
  if (gIsSinglePlayer) {
    gP2Position.y += AI_SPEED * gAIDirection * deltaTime;
    if (gP2Position.y - PADDLE_SCALE.y / 2.0f <= 0.0f) {
      gAIDirection = 1.0f;
    } 
    if (gP2Position.y + PADDLE_SCALE.y / 2.0f >= SCREEN_HEIGHT) {
      gAIDirection = -1.0f;
    }
  }
  else {
    gP2Position.y += PADDLE_SPEED * gP2Movement.y * deltaTime;
  }

  float minY = PADDLE_SCALE.y / 2.0f;
  float maxY = SCREEN_HEIGHT - PADDLE_SCALE.y / 2.0f;
  if (gP1Position.y < minY) gP1Position.y = minY;
  if (gP1Position.y > maxY) gP1Position.y = maxY;
  if (gP2Position.y < minY) gP2Position.y = minY;
  if (gP2Position.y > maxY) gP2Position.y = maxY;

  // Balls
  for (int i = 0; i < gActiveBalls; i++) {
    gBallPositions[i].x += BALL_SPEED * gBallMovements[i].x * deltaTime;
    gBallPositions[i].y += BALL_SPEED * gBallMovements[i].y * deltaTime;

    if (gBallPositions[i].y - BALL_SCALE.y / 2.0f <= 0.0f) {
      gBallPositions[i].y = BALL_SCALE.y / 2.0f;
      gBallMovements[i].y = fabs(gBallMovements[i].y);
    }
    if (gBallPositions[i].y + BALL_SCALE.y / 2.0f >= SCREEN_HEIGHT) {
      gBallPositions[i].y = SCREEN_HEIGHT - BALL_SCALE.y / 2.0f;
      gBallMovements[i].y = -fabs(gBallMovements[i].y);
    }

    // Bounce off paddles
    if (isColliding(&gBallPositions[i], &BALL_SCALE, 
        &gP1Position, &PADDLE_SCALE)) {
      gBallMovements[i].x = fabs(gBallMovements[i].x);
    }
    if (isColliding(&gBallPositions[i], &BALL_SCALE, 
        &gP2Position, &PADDLE_SCALE)) {
      gBallMovements[i].x = -fabs(gBallMovements[i].x);
    }

    // Scoring
    if (gBallPositions[i].x < 0.0f) { 
      gP2Score++;
      resetBall(i, -1.0f);
    }
    else if (gBallPositions[i].x > SCREEN_WIDTH) {
      gP1Score++;
      resetBall(i, 1.0f);
    }
  }

  // Game over
  if (gP1Score >= WIN_SCORE || gP2Score >= WIN_SCORE) gGameStatus = GAME_OVER;
}

void render() {
  BeginDrawing();
  ClearBackground(ColorFromHex(BG_COLOR));

  // Scores
  Vector2 p1ScorePosition = {ORIGIN.x - 60.0f, 40.0f};
  Vector2 p2ScorePosition = {ORIGIN.x + 60.0f, 40.0f};
  renderObject(&gDigitTextures[gP1Score % 10], 
    &p1ScorePosition, &DIGIT_SCALE, WHITE);
  renderObject(&gDigitTextures[gP2Score % 10], 
    &p2ScorePosition, &DIGIT_SCALE, WHITE);

  // Paddles
  const Texture2D *p2Texture = gIsSinglePlayer ? 
    &gPaddleBoltTexture : &gPaddleTexture;
  renderObject(&gPaddleTexture, &gP1Position, &PADDLE_SCALE, 
    ColorFromHex(P1_COLOR));
  renderObject(p2Texture, &gP2Position, &PADDLE_SCALE, 
    ColorFromHex(P2_COLOR));

  // Balls
  for (int i = 0; i < gActiveBalls; i++) {
    renderObject(&gBallTexture, &gBallPositions[i], &BALL_SCALE, WHITE);
  }

  // Winner star
  if (gGameStatus == GAME_OVER) {
    Vector2 starPosition = {
      (gP1Score >= WIN_SCORE) ? 
      SCREEN_WIDTH * 0.25f : SCREEN_WIDTH * 0.75f, ORIGIN.y
    };
    renderObject(&gStarTexture, &starPosition, &STAR_SCALE, WHITE);
  }

  EndDrawing();
}

void shutdown() {
  UnloadTexture(gPaddleTexture);
  UnloadTexture(gPaddleBoltTexture);
  UnloadTexture(gBallTexture);
  UnloadTexture(gStarTexture);
  for (int i = 0; i < 10; i++) UnloadTexture(gDigitTextures[i]);

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