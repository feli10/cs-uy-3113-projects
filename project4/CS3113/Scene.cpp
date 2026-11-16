#include "Scene.h"

Scene::Scene() : mGameState{}, mOrigin{} {}

Scene::Scene(Vector2 origin, const char *bgHexCode)
    : mGameState{}, mOrigin{ origin }, mBGColourHexCode{ bgHexCode }
{}

// Creates an enemy standing in map tile (col, row)
void Scene::addEnemy(AIType type, int col, int row) {
    if (mGameState.enemyCount >= MAX_ENEMIES) return;

    std::map<Direction, std::vector<int>> atlas;
    int  speed;
    bool hasGravity = true;

    switch (type) {
    case CHASER: // ghost
        atlas = {
            { LEFT,  { 320, 321, 322, 323 } },
            { RIGHT, { 320, 321, 322, 323 } },
        };
        speed      = Entity::GHOST_SPEED;
        hasGravity = false;
        break;
    case RUNNER: // little coward
        atlas = {
            { LEFT,  { 246, 266 } },
            { RIGHT, { 246, 266 } },
        };
        speed = Entity::RUNNER_SPEED;
        break;
    default: // goomba-style walker
        atlas = {
            { LEFT,  { 360, 361, 362, 363 } },
            { RIGHT, { 360, 361, 362, 363 } },
        };
        speed = Entity::GOOMBA_SPEED;
        break;
    }

    float tileSize = mGameState.map->getTileSize();

    Entity *enemy = new Entity(
        getTileCentre(col, row),
        { tileSize, tileSize },
        SPRITESHEET_PATH,
        ATLAS,
        { SPRITESHEET_ROWS, SPRITESHEET_COLS },
        atlas,
        NPC
    );

    enemy->setAIType(type);
    enemy->setAIState(IDLE);
    enemy->setSpeed(speed);
    enemy->setFrameSpeed(Entity::ENEMY_FRAME_SPEED);
    enemy->setColliderDimensions({ tileSize * 0.7f, tileSize });
    if (hasGravity) enemy->setAcceleration({ 0.0f, GRAVITY });
    mGameState.enemies[mGameState.enemyCount] = enemy;
    mGameState.enemyCount++;
}

// Moves every enemy then checks each one against the player
void Scene::updateEnemies(float deltaTime) {
    for (int i = 0; i < mGameState.enemyCount; i++) {
        Entity *enemy = mGameState.enemies[i];
        if (!enemy->isActive()) continue;

        Map *map = (enemy->getAIType() == CHASER) ? nullptr : mGameState.map;
        enemy->update(deltaTime, mGameState.critter, map, nullptr, 0);

        if (enemy->getPosition().y > mGameState.map->getBottomBoundary() +
            mGameState.map->getTileSize()) {
            // fell into pit
            enemy->deactivate();
            continue;
        }

        if (!mGameState.critter->isTouching(enemy)) continue;

        if (enemy->getAIType() == RUNNER) {
            mGameState.gotExtraLife = true;
            enemy->deactivate();
        }
        else if (!mGameState.critter->isInvincible()) {
            // Lose a life but keep playing, untouchable for a moment
            mGameState.wasPlayerHit = true;
            mGameState.critter->makeInvincible(INVINCIBILITY_TIME);
        }
    }
}

void Scene::renderEnemies() {
    for (int i = 0; i < mGameState.enemyCount; i++) {
        mGameState.enemies[i]->render();
    }
}

void Scene::deleteEnemies() {
    for (int i = 0; i < mGameState.enemyCount; i++) {
        delete mGameState.enemies[i];
        mGameState.enemies[i] = nullptr;
    }
    mGameState.enemyCount = 0;
}
