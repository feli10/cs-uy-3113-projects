#ifndef SCENE_H
#define SCENE_H

#include "Entity.h"

constexpr char SPRITESHEET_PATH[] =
  "assets/kenney_1-bit-platformer-pack/Tilemap/monochrome_tilemap_transparent_packed.png";
constexpr int SPRITESHEET_COLS = 20;
constexpr int SPRITESHEET_ROWS = 20;

constexpr int CRITTER_STAND_FRAME = 241, // facing right
              CRITTER_STEP_FRAME  = 242,
              CRITTER_STEP2_FRAME = 243,
              CRITTER_JUMP_FRAME  = 244, // arms out, shown while airborne
              EXIT_DOOR_FRAME     = 58;  // open door at the end of each level

constexpr float GRAVITY = 981.0f;

constexpr int MAX_ENEMIES = 10;

// After an enemy hit the player flickers and can't be hurt for this long
constexpr float INVINCIBILITY_TIME = 1.5f;

struct GameState
{
    Entity *critter;
    Map    *map;
    Entity *exitDoor;
    Entity *enemies[MAX_ENEMIES];
    int     enemyCount;

    SceneID nextSceneID;

    bool isPlayerDead;
    bool wasPlayerHit;
    bool gotExtraLife;
};

class Scene
{
protected:
    GameState   mGameState;
    Vector2     mOrigin;
    const char *mBGColourHexCode = "#000000";

    // World position of the centre of map tile
    Vector2 getTileCentre(int col, int row) const
    {
        float tileSize = mGameState.map->getTileSize();
        return {
            mGameState.map->getLeftBoundary() + (col + 0.5f) * tileSize,
            mGameState.map->getTopBoundary()  + (row + 0.5f) * tileSize
        };
    }

    void addEnemy(AIType type, int col, int row);

    void updateEnemies(float deltaTime);

    void renderEnemies();

    void deleteEnemies();

public:
    Scene();
    Scene(Vector2 origin, const char *bgHexCode);
    virtual ~Scene() {}

    virtual void initialise() = 0;
    virtual void processInput() {}
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;

    void clearExtraLife() { mGameState.gotExtraLife = false; }
    void clearPlayerHit() { mGameState.wasPlayerHit = false; }

    GameState   getState()           const { return mGameState;        }
    Vector2     getOrigin()          const { return mOrigin;           }
    const char *getBGColourHexCode() const { return mBGColourHexCode; }
};

#endif // SCENE_H
