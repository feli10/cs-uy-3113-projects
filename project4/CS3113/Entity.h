#ifndef ENTITY_H
#define ENTITY_H

#include "Map.h"

enum Direction    { LEFT, UP, RIGHT, DOWN              }; // For walking
enum EntityStatus { ACTIVE, INACTIVE                   };
enum EntityType   { PLAYER, BLOCK, PLATFORM, NPC, NONE };
enum AIType       { WANDERER, FOLLOWER, CHASER, RUNNER  };
enum AIState      { WALKING, IDLE, FOLLOWING, FLEEING, AIMING, LUNGING };

class Entity
{
private:
    Vector2 mPosition;
    Vector2 mMovement;
    Vector2 mVelocity;
    Vector2 mAcceleration;

    Vector2 mScale;
    Vector2 mColliderDimensions;

    Texture2D mTexture;
    TextureType mTextureType;
    Vector2 mSpriteSheetDimensions;

    std::map<Direction, std::vector<int>> mAnimationAtlas;
    std::vector<int>                      mAnimationIndices;
    Direction                             mDirection;
    int                                   mFrameSpeed;

    int mCurrentFrameIndex = 0;
    float mAnimationTime = 0.0f;

    bool mIsJumping = false;
    float mJumpingPower = 0.0f;
    int mJumpFrame = -1;

    float mInvincibleTime = 0.0f; 
    float mAITimer = 0.0f; 

    int mSpeed;
    float mAngle;

    bool mIsCollidingTop    = false;
    bool mIsCollidingBottom = false;
    bool mIsCollidingRight  = false;
    bool mIsCollidingLeft   = false;

    EntityStatus mEntityStatus = ACTIVE;
    EntityType   mEntityType;

    AIType  mAIType;
    AIState mAIState;

    bool isColliding(Entity *other) const;

    void checkCollisionY(Entity *collidableEntities, int collisionCheckCount);
    void checkCollisionY(Map *map);

    void checkCollisionX(Entity *collidableEntities, int collisionCheckCount);
    void checkCollisionX(Map *map);

    void resetColliderFlags()
    {
        mIsCollidingTop    = false;
        mIsCollidingBottom = false;
        mIsCollidingRight  = false;
        mIsCollidingLeft   = false;
    }

    void animate(float deltaTime);
    void AIActivate(Entity *target, Map *map, float deltaTime);
    void AIWander(Map *map);
    void AIFollow(Entity *target);
    void AIChase(Entity *target, float deltaTime);
    void AIFlee(Entity *target);
    bool isBlockedAhead(Map *map) const;

public:
    static constexpr int   DEFAULT_SIZE          = 250;
    static constexpr int   DEFAULT_SPEED         = 200;
    static constexpr int   DEFAULT_FRAME_SPEED   = 14;
    static constexpr float Y_COLLISION_THRESHOLD = 0.5f;

    // Enemy tuning
    static constexpr int   ENEMY_FRAME_SPEED     = 8;

    static constexpr int   GOOMBA_SPEED          = 100;

    static constexpr int   GHOST_SPEED           = 200; 
    static constexpr float CHASE_RANGE           = 450.0f;
    static constexpr float LUNGE_AIM_TIME        = 1.0f;
    static constexpr float LUNGE_TIME            = 0.8f;

    static constexpr int   RUNNER_SPEED          = 120;
    static constexpr float FLEE_RANGE            = 250.0f;

    Entity();
    Entity(
        Vector2     position,
        Vector2     scale,
        const char *textureFilepath,
        EntityType  entityType
    );
    Entity(
        Vector2                               position,
        Vector2                               scale,
        const char                           *textureFilepath,
        TextureType                           textureType,
        Vector2                               spriteSheetDimensions,
        std::map<Direction, std::vector<int>> animationAtlas,
        EntityType                            entityType
    );
    ~Entity();

    void update(
        float   deltaTime,
        Entity *player,
        Map    *map,
        Entity *collidableEntities,
        int     collisionCheckCount
    );
    void render();
    void normaliseMovement() { Normalise(&mMovement); }

    void jump()       { mIsJumping = true;         }
    void activate()   { mEntityStatus = ACTIVE;    }
    void deactivate() { mEntityStatus = INACTIVE;  }
    void displayCollider();

    bool isActive() { return mEntityStatus == ACTIVE; }
    bool isInvincible() const { return mInvincibleTime > 0.0f; }
    void makeInvincible(float seconds) { mInvincibleTime = seconds; }
    // Player's-eye collision check: is this entity touching `other`?
    bool isTouching(Entity *other) const { return isColliding(other); }

    void moveUp()    { mMovement.y = -1; mDirection = UP;    }
    void moveDown()  { mMovement.y =  1; mDirection = DOWN;  }
    void moveLeft()  { mMovement.x = -1; mDirection = LEFT;  }
    void moveRight() { mMovement.x =  1; mDirection = RIGHT; }

    void resetMovement() { mMovement = { 0.0f, 0.0f }; }

    Vector2     getPosition()              const { return mPosition;              }
    Vector2     getMovement()              const { return mMovement;              }
    Vector2     getVelocity()              const { return mVelocity;              }
    Vector2     getAcceleration()          const { return mAcceleration;          }
    Vector2     getScale()                 const { return mScale;                 }
    Vector2     getColliderDimensions()    const { return mColliderDimensions;    }
    Vector2     getSpriteSheetDimensions() const { return mSpriteSheetDimensions; }
    Texture2D   getTexture()               const { return mTexture;               }
    TextureType getTextureType()           const { return mTextureType;           }
    Direction   getDirection()             const { return mDirection;             }
    int         getFrameSpeed()            const { return mFrameSpeed;            }
    float       getJumpingPower()          const { return mJumpingPower;          }
    bool        isJumping()                const { return mIsJumping;             }
    int         getSpeed()                 const { return mSpeed;                 }
    float       getAngle()                 const { return mAngle;                 }
    EntityType  getEntityType()            const { return mEntityType;            }
    AIType      getAIType()                const { return mAIType;                }
    AIState     getAIState()               const { return mAIState;               }

    bool isCollidingTop()    const { return mIsCollidingTop;    }
    bool isCollidingBottom() const { return mIsCollidingBottom; }

    std::map<Direction, std::vector<int>> getAnimationAtlas() const { return mAnimationAtlas; }

    void setPosition(Vector2 newPosition)
        { mPosition = newPosition;                 }
    void setMovement(Vector2 newMovement)
        { mMovement = newMovement;                 }
    void setAcceleration(Vector2 newAcceleration)
        { mAcceleration = newAcceleration;         }
    void setScale(Vector2 newScale)
        { mScale = newScale;                       }
    void setTexture(const char *textureFilepath)
        { mTexture = LoadTexture(textureFilepath); }
    void setColliderDimensions(Vector2 newDimensions)
        { mColliderDimensions = newDimensions;     }
    void setSpriteSheetDimensions(Vector2 newDimensions)
        { mSpriteSheetDimensions = newDimensions;  }
    void setSpeed(int newSpeed)
        { mSpeed = newSpeed;                       }
    void setFrameSpeed(int newSpeed)
        { mFrameSpeed = newSpeed;                  }
    void setJumpingPower(float newJumpingPower)
        { mJumpingPower = newJumpingPower;         }
    void setJumpFrame(int newJumpFrame)
        { mJumpFrame = newJumpFrame;               }
    void setAngle(float newAngle)
        { mAngle = newAngle;                       }
    void setEntityType(EntityType entityType)
        { mEntityType = entityType;                }
    void setAIState(AIState newState)
        { mAIState = newState;                     }
    void setAIType(AIType newType)
        { mAIType = newType;                       }
    void setDirection(Direction newDirection)
    {
        mDirection = newDirection;

        if (mTextureType == ATLAS)
            mAnimationIndices = mAnimationAtlas.at(mDirection);
    }
};

#endif // ENTITY_H
