/**
* Author: Xufei (Felix) Zhang
* Assignment: Rise of the AI
* Date due: 11/16/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/LevelA.h"
#include "CS3113/LevelB.h"
#include "CS3113/LevelC.h"
#include "CS3113/MessageScene.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 600,
              FPS           = 120,
              MAX_LIVES     = 3;

constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

constexpr char GAME_TITLE[] = "= Simple Platformer =";

constexpr char BGM_PATH[] = "assets/sergequadrado-ethereal-uplifting-loop-275529.mp3";
constexpr char JUMP_SOUND_PATH[] = "assets/kenney_digital-audio/Audio/phaseJump1.ogg";
constexpr char HURT_SOUND_PATH[] = "assets/kenney_digital-audio/Audio/lowDown.ogg";
constexpr char ENEMY_HIT_SOUND_PATH[] = "assets/kenney_digital-audio/Audio/spaceTrash4.ogg";
constexpr char LEVEL_COMPLETE_SOUND_PATH[] = "assets/kenney_digital-audio/Audio/powerUp1.ogg";
constexpr char EXTRA_LIFE_SOUND_PATH[] = "assets/kenney_digital-audio/Audio/powerUp7.ogg";

// Global Variables
AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f;
float gTimeAccumulator = 0.0f;

int gLives = MAX_LIVES;

Music gBGM;
Sound gJumpSound;
Sound gHurtSound;      // fell in a pit
Sound gEnemyHitSound;  // touched an enemy
Sound gLevelCompleteSound;
Sound gExtraLifeSound;

Camera2D gCamera = {};

Scene *gCurrentScene = nullptr;
std::map<SceneID, Scene *> gLevels = {};

// Function Declarations
Vector2 getCameraTarget(GameState state);
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

// Follow the player sideways
Vector2 getCameraTarget(GameState state) {
  if (state.critter == nullptr) return ORIGIN;

  float x = state.critter->getPosition().x;
  float minX = state.map->getLeftBoundary() + ORIGIN.x;
  float maxX = state.map->getRightBoundary() - ORIGIN.x;
  if (x < minX) x = minX;
  if (x > maxX) x = maxX;

  return { x, ORIGIN.y };
}

void switchToScene(Scene *scene) {
  if (gCurrentScene) gCurrentScene->shutdown();
  gCurrentScene = scene;
  gCurrentScene->initialise();

  // restart
  if (gCurrentScene == gLevels[MENU]) gLives = MAX_LIVES;

  // Snap straight to the new scene instead of panning across from the old one
  gCamera.target = getCameraTarget(gCurrentScene->getState());
}

void initialise() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Rise of the AI");
  InitAudioDevice();

  gBGM = LoadMusicStream(BGM_PATH);
  SetMusicVolume(gBGM, 0.33f);
  PlayMusicStream(gBGM);

  gJumpSound = LoadSound(JUMP_SOUND_PATH);
  gHurtSound = LoadSound(HURT_SOUND_PATH);
  gEnemyHitSound = LoadSound(ENEMY_HIT_SOUND_PATH);
  gLevelCompleteSound = LoadSound(LEVEL_COMPLETE_SOUND_PATH);
  gExtraLifeSound = LoadSound(EXTRA_LIFE_SOUND_PATH);

  gLevels[MENU] = new MessageScene(ORIGIN, "#151524", GAME_TITLE, 
    "Press Enter", "#e7c041", LEVEL_A);
  gLevels[LEVEL_A] = new LevelA(ORIGIN, "#2f2623");
  gLevels[LEVEL_B] = new LevelB(ORIGIN, "#021423");
  gLevels[LEVEL_C] = new LevelC(ORIGIN, "#200930");
  gLevels[WIN] = new MessageScene(ORIGIN, "#0b4131", "YOU WIN", 
    "Press Enter", "#5cd478", MENU);
  gLevels[LOSE] = new MessageScene(ORIGIN, "#400e0e", "YOU PERISHED", 
    "Press Enter", "#e94646", MENU);

  gCamera.offset = ORIGIN;
  gCamera.rotation = 0.0f;
  gCamera.zoom = 1.0f;

  switchToScene(gLevels[MENU]);

  SetTargetFPS(FPS);
}

void processInput() {
  if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;

  // jump to a level with the number keys
  if (IsKeyPressed(KEY_ONE)) { switchToScene(gLevels[LEVEL_A]); return; }
  if (IsKeyPressed(KEY_TWO)) { switchToScene(gLevels[LEVEL_B]); return; }
  if (IsKeyPressed(KEY_THREE)) { switchToScene(gLevels[LEVEL_C]); return; }

  gCurrentScene->processInput();

  // player control
  Entity *player = gCurrentScene->getState().critter;
  if (player == nullptr) return;

  player->resetMovement();

  if (IsKeyDown(KEY_A)) player->moveLeft();
  else if (IsKeyDown(KEY_D)) player->moveRight();

  if (IsKeyPressed(KEY_W) && player->isCollidingBottom()) {
      player->jump();
      PlaySound(gJumpSound);
  }

  if (GetLength(player->getMovement()) > 1.0f) player->normaliseMovement();
}

void update() {
  // Delta time
  float ticks     = static_cast<float>(GetTime());
  float deltaTime = ticks - gPreviousTicks;
  gPreviousTicks  = ticks;
  deltaTime += gTimeAccumulator;

  if (deltaTime < FIXED_TIMESTEP) {
    gTimeAccumulator = deltaTime;
    return;
  }

  while (deltaTime >= FIXED_TIMESTEP) {
    gCurrentScene->update(FIXED_TIMESTEP);
    deltaTime -= FIXED_TIMESTEP;

    Vector2 cameraTarget = getCameraTarget(gCurrentScene->getState());
    panCamera(&gCamera, &cameraTarget);
  }

  gTimeAccumulator = deltaTime;
}

void render() {
  BeginDrawing();
  BeginMode2D(gCamera);

  gCurrentScene->render();

  EndMode2D();

  if (gCurrentScene->getState().critter != nullptr) {
    DrawText(TextFormat("LIVES: %d", gLives), 20, 20, 24, WHITE);
  }

  EndDrawing();
}

void shutdown() {
  for (std::pair<const SceneID, Scene *> &entry : gLevels)
    delete entry.second;

  gLevels.clear();

  UnloadMusicStream(gBGM);
  UnloadSound(gJumpSound);
  UnloadSound(gHurtSound);
  UnloadSound(gEnemyHitSound);
  UnloadSound(gLevelCompleteSound);
  UnloadSound(gExtraLifeSound);
  CloseAudioDevice();
  CloseWindow();
}

int main(void) {
  initialise();

  while (gAppStatus == RUNNING) {
    UpdateMusicStream(gBGM);

    processInput();
    update();

    GameState state = gCurrentScene->getState();

    // Caught a runner
    if (state.gotExtraLife) {
      gLives++;
      PlaySound(gExtraLifeSound);
      gCurrentScene->clearExtraLife();
    }

    // Hit by an enemy
    bool lostLife = false;
    if (state.wasPlayerHit) {
      gLives--;
      lostLife = true;
      PlaySound(gEnemyHitSound);
      gCurrentScene->clearPlayerHit();
    }

    // Fell off the map
    if (state.isPlayerDead) {
      gLives--;
      lostLife = true;
      PlaySound(gHurtSound);
    }

    if (lostLife && gLives <= 0) switchToScene(gLevels[LOSE]);
    else if (state.isPlayerDead) switchToScene(gCurrentScene);
    else if (state.nextSceneID != NO_SCENE) {
      if (state.critter != nullptr) PlaySound(gLevelCompleteSound);
      switchToScene(gLevels[state.nextSceneID]);
    }

    render();
  }

  shutdown();

  return 0;
}
