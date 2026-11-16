#ifndef MESSAGESCENE_H
#define MESSAGESCENE_H

#include "Scene.h"

class MessageScene : public Scene {
private:
  const char *mTitle;
  const char *mSubtitle;
  const char *mTitleColourHexCode;
  SceneID     mTargetScene;

public:
  static constexpr int TITLE_FONT_SIZE = 60;
  static constexpr int SUBTITLE_FONT_SIZE = 24;

  MessageScene(Vector2 origin, const char *bgHexCode, const char *title,
    const char *subtitle, const char *titleHexCode, SceneID targetScene);
  ~MessageScene();
  void initialise() override;
  void processInput() override;
  void update(float deltaTime) override;
  void render() override;
  void shutdown() override;
};

#endif
