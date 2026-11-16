#include "LevelC.h"

LevelC::LevelC() : Scene{ { 0.0f, 0.0f }, "#000000" } {}

LevelC::LevelC(Vector2 origin, const char *bgHexCode) 
  : Scene{ origin, bgHexCode } {}

LevelC::~LevelC() { shutdown(); }

void LevelC::initialise() {
  mGameState.nextSceneID = NO_SCENE;
  mGameState.isPlayerDead = false;
  mGameState.gotExtraLife = false;
  mGameState.wasPlayerHit = false;

  /*
    ----------- MAP -----------
  */
  Vector2 mapOrigin = { LEVEL_WIDTH * TILE_DIMENSION / 2.0f, mOrigin.y };

  mGameState.map = new Map(
    LEVEL_WIDTH, LEVEL_HEIGHT, // map grid cols & rows
    mLevelData,                // grid data
    SPRITESHEET_PATH,          // texture filepath
    TILE_DIMENSION,            // tile size
    SPRITESHEET_COLS,          // texture cols
    SPRITESHEET_ROWS,          // texture rows
    mapOrigin                  // in-game origin
  );

  /*
    ----------- PROTAGONIST -----------
  */
  std::map<Direction, std::vector<int>> critterAnimationAtlas = {
    { LEFT,  { CRITTER_STAND_FRAME, CRITTER_STEP_FRAME,
      CRITTER_STAND_FRAME, CRITTER_STEP2_FRAME } },
    { RIGHT, { CRITTER_STAND_FRAME, CRITTER_STEP_FRAME,
      CRITTER_STAND_FRAME, CRITTER_STEP2_FRAME } },
  };

  mGameState.critter = new Entity(
    getTileCentre(2, 4),                         // position
    { TILE_DIMENSION, TILE_DIMENSION },          // scale (one tile)
    SPRITESHEET_PATH,                            // texture file address
    ATLAS,                                       // single image or atlas?
    { SPRITESHEET_ROWS, SPRITESHEET_COLS },      // atlas dimensions
    critterAnimationAtlas,                       // actual atlas
    PLAYER                                       // entity type
  );

  mGameState.critter->setJumpingPower(550.0f);
  mGameState.critter->setJumpFrame(CRITTER_JUMP_FRAME);
  // A bit narrower than the sprite so the player fits through gaps
  mGameState.critter->setColliderDimensions({
    mGameState.critter->getScale().x * 0.7f,
    mGameState.critter->getScale().y
  });
  mGameState.critter->setAcceleration({ 0.0f, GRAVITY });

  /*
    ----------- EXIT DOOR -----------
  */
  std::map<Direction, std::vector<int>> doorAtlas = {
    { LEFT,  { EXIT_DOOR_FRAME } },
    { RIGHT, { EXIT_DOOR_FRAME } },
  };

  mGameState.exitDoor = new Entity(
    getTileCentre(LEVEL_WIDTH - 3, 4),           // on the ground, by the wall
    { TILE_DIMENSION, TILE_DIMENSION },
    SPRITESHEET_PATH,
    ATLAS,
    { SPRITESHEET_ROWS, SPRITESHEET_COLS },
    doorAtlas,
    NONE
  );

  /*
    ----------- ENEMIES -----------
  */
  addEnemy(WANDERER, 54, 4);
  addEnemy(WANDERER, 35, 4);
  addEnemy(CHASER,   15, 1);
  addEnemy(CHASER,   45, 1);
  addEnemy(WANDERER,   21, 4);
  addEnemy(RUNNER,   27, 6);  // at the back of the cave under the staircase
}

void LevelC::update(float deltaTime) {
  mGameState.critter->update(
    deltaTime,      // delta time / fixed timestep
    nullptr,        // player
    mGameState.map, // map
    nullptr,        // collidable entities
    0               // col. entity count
  );

  // Fell off the map -> lose a life
  if (mGameState.critter->getPosition().y > END_GAME_THRESHOLD) {
      mGameState.isPlayerDead = true;
  }

  // Enemies move, and touching one removes or grants a life
  updateEnemies(deltaTime);

  // Game won!
  if (mGameState.critter->getPosition().x >= mGameState.exitDoor->getPosition().x) {
    mGameState.nextSceneID = WIN;
  }
}

void LevelC::render() {
  ClearBackground(ColorFromHex(mBGColourHexCode));

  mGameState.map->render();
  mGameState.exitDoor->render();
  renderEnemies();
  mGameState.critter->render();
}

void LevelC::shutdown() {
  delete mGameState.critter;
  mGameState.critter = nullptr;
  deleteEnemies();
  delete mGameState.exitDoor;
  mGameState.exitDoor = nullptr;
  delete mGameState.map;
  mGameState.map = nullptr;
}
