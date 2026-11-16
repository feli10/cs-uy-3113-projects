#include "MessageScene.h"

MessageScene::MessageScene(Vector2 origin, const char *bgHexCode,
  const char *title, const char *subtitle, const char *titleHexCode,
  SceneID targetScene) : Scene{ origin, bgHexCode }, mTitle{ title },
  mSubtitle{ subtitle }, mTitleColourHexCode{ titleHexCode },
  mTargetScene{ targetScene } {}

MessageScene::~MessageScene() { shutdown(); }

void MessageScene::initialise() {
  mGameState.critter      = nullptr;
  mGameState.map          = nullptr;
  mGameState.exitDoor     = nullptr;
  mGameState.nextSceneID  = NO_SCENE;
  mGameState.isPlayerDead = false;
  mGameState.gotExtraLife = false;
  mGameState.wasPlayerHit = false;
  mGameState.enemyCount   = 0;
}

void MessageScene::processInput() {
  if (IsKeyPressed(KEY_ENTER)) mGameState.nextSceneID = mTargetScene;
}

void MessageScene::update(float deltaTime) {}

void MessageScene::render() {
  ClearBackground(ColorFromHex(mBGColourHexCode));

  DrawText(mTitle, mOrigin.x - MeasureText(mTitle, TITLE_FONT_SIZE) / 2.0f,
    mOrigin.y - 60.0f, TITLE_FONT_SIZE, ColorFromHex(mTitleColourHexCode));

  DrawText(mSubtitle, mOrigin.x - MeasureText(mSubtitle, SUBTITLE_FONT_SIZE)
    / 2.0f, mOrigin.y + 30.0f, SUBTITLE_FONT_SIZE, WHITE);
}

void MessageScene::shutdown() {}
