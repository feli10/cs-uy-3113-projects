#include "CS3113/Entity.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 800,
              FPS           = 120;

constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

constexpr char BG_COLOUR[] = "#000000";
constexpr Vector2 ORIGIN = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};

constexpr Vector2 PLANET_SCALE    = {200.0f, 200.0f},
                  MOON_SCALE      = {80.0f, 80.0f},
                  SATELLITE_SCALE = {20.0f, 20.0f},
                  ROCKET_SCALE    = {40.0f, 40.0f},
                  FLAME_SCALE     = {40.0f, 40.0f};

constexpr float MOON_ORBIT_RADIUS = 300.0f;
constexpr float MOON_ORBIT_SPEED = 0.2f;

constexpr Vector2 ROCKET_START_POS = {50.0f, 200.0f};
constexpr Vector2 MOON_START_POS = {ORIGIN.x + MOON_ORBIT_RADIUS, ORIGIN.y};

constexpr Vector2 ROCKET_START_VELOCITY = {100.0f, -100.0f};

constexpr int NUMBER_OF_SATELLITES = 3;
constexpr float SATELLITE_START_ANGLES[NUMBER_OF_SATELLITES] = {0.0f, 2.1f, 4.2f};
constexpr float SATELLITE_ORBIT_RADII[NUMBER_OF_SATELLITES] = {150.0f, 200.0f, 240.0f};
constexpr float SATELLITE_ORBIT_SPEEDS[NUMBER_OF_SATELLITES] = {0.6f, -0.4f, 0.3f};
constexpr Vector2 SATELLITE_COLLIDER = {12.0f, 12.0f};

constexpr char PLANET_PATH[] = "assets/kenney_planets/Planets/planet03.png";
constexpr char MOON_PATH[] = "assets/kenney_planets/Planets/planet04.png";
constexpr char SATELLITE_PATH[] = "assets/kenney_simple-space/PNG/Default/satellite_A.png";
constexpr char ROCKET_PATH[] = "assets/kenney_simple-space/PNG/Default/ship_L.png";
constexpr char FLAME_PATH[] = "assets/kenney_simple-space/PNG/Default/effect_yellow.png";
constexpr char BACKGROUND_PATH[] = "assets/kenney_skyboxes-space/Skyboxes/skybox-space-dark.png";

constexpr char THRUST_SOUND_PATH[] = "assets/kenney_sci-fi-sounds/Audio/thrusterFire_003.ogg";
constexpr char EXPLOSION_SOUND_PATH[] = "assets/kenney_sci-fi-sounds/Audio/explosionCrunch_002.ogg";

constexpr char EXPLOSION_PATH[] = "assets/explosion_sheet.png";
constexpr int EXPLOSION_FRAMES = 9;
constexpr int EXPLOSION_FPS = 20;
constexpr Vector2 EXPLOSION_SCALE = {100.0f, 100.0f};

constexpr float GRAVITY = 40.0f;
constexpr float THRUST = 100.0f;
constexpr float DRAG = 0.2f;
constexpr float ROTATION_SPEED = 100.0f;

constexpr float SAFE_LANDING_SPEED = 50.0f;
constexpr float SAFE_SHEAR_SPEED = 100.0f;
constexpr float SAFE_LANDING_ANGLE = 20.0f;

constexpr float FLAME_FADE_SPEED = 5.0f;
constexpr float FLAME_OVERLAP = 6.0f;

constexpr float MAX_FUEL = 100.0f;
constexpr float FUEL_BURN_RATE = 10.0f;

// Global Variables
enum GameStatus {PLAYING, WON, LOST};
enum LossReason {NO_LOSS, OFF_SCREEN, COLLISION, CRASH_LANDING};
enum ContactResult {NO_CONTACT, SAFE_LANDING, CRASH};

AppStatus gAppStatus = RUNNING;
GameStatus gGameStatus = PLAYING;
LossReason gLossReason = NO_LOSS;
ContactResult gContactResult = NO_CONTACT;
float gPreviousTicks = 0.0f;
float gTimeAccumulator = 0.0f;

Entity *gBackground = nullptr;
Entity *gPlanet = nullptr;
Entity *gRocket = nullptr;
Entity *gMoon = nullptr;
Entity *gSatellites = nullptr;
Entity *gFlame = nullptr;
Entity *gExplosion = nullptr;
float gExplosionTimer = 0.0f;

constexpr int NUMBER_OF_LANDABLE_BODIES = 2;
Entity *gLandableBodies[NUMBER_OF_LANDABLE_BODIES];

float gRotate = 0.0f;
bool gIsThrusting = false;

float gMoonAngle = 0.0f;
float gSatelliteAngles[NUMBER_OF_SATELLITES];

float gFuel = MAX_FUEL;

Sound gThrustSound;
Sound gExplosionSound;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

void resetGame();
ContactResult checkLanding(Entity *bodies[], int bodyCount);
void moveSatellites(float deltaTime);
bool isTouchingAny(Entity *bodies, int bodyCount);

void initialise() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Project 3");
  InitAudioDevice();

  // Sounds
  gThrustSound = LoadSound(THRUST_SOUND_PATH);
  SetSoundVolume(gThrustSound, 0.5f);
  gExplosionSound = LoadSound(EXPLOSION_SOUND_PATH);
  SetSoundVolume(gThrustSound, 0.8f);

  // Background
  gBackground = new Entity(ORIGIN,
    {static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT)},
    BACKGROUND_PATH, NONE);
  gBackground->setOpacity(0.5f);
  gPlanet = new Entity(ORIGIN, PLANET_SCALE, PLANET_PATH, PLANET);
  gMoon = new Entity(MOON_START_POS, MOON_SCALE, MOON_PATH, HAZARD);
  gRocket = new Entity(ROCKET_START_POS, ROCKET_SCALE, ROCKET_PATH, PLAYER);

  // Satellites
  gSatellites = new Entity[NUMBER_OF_SATELLITES];
  for (int i = 0; i < NUMBER_OF_SATELLITES; i++) {
    gSatellites[i].setTexture(SATELLITE_PATH);
    gSatellites[i].setScale(SATELLITE_SCALE);
    gSatellites[i].setColliderDimensions(SATELLITE_COLLIDER);
    gSatellites[i].setEntityType(HAZARD);
    // gSatellites[i].setColliderVisible(true);
    gSatelliteAngles[i] = SATELLITE_START_ANGLES[i];
  }
  moveSatellites(0.0f);

  gPlanet->setColliderDimensions({160.0f, 160.0f});
  gMoon->setColliderDimensions({60.0f, 60.0f});
  gRocket->setColliderDimensions({30.0f, 30.0f});
  // gPlanet->setColliderVisible(true);
  // gMoon->setColliderVisible(true);
  // gRocket->setColliderVisible(true);

  gRocket->setVelocity(ROCKET_START_VELOCITY);

  gLandableBodies[0] = gPlanet;
  gLandableBodies[1] = gMoon;

  // Flame starts invisible and fades in while thrusting
  gFlame = new Entity(ROCKET_START_POS, FLAME_SCALE, FLAME_PATH, NONE);
  gFlame->setOpacity(0.0f);

  // Explosion
  std::map<Direction, std::vector<int>> explosionAtlas = {
    {RIGHT, {0, 1, 2, 3, 4, 5, 6, 7, 8}}
  };
  gExplosion = new Entity(ORIGIN, EXPLOSION_SCALE, EXPLOSION_PATH, ATLAS,
    {1, EXPLOSION_FRAMES}, explosionAtlas, NONE);
  gExplosion->setFrameSpeed(EXPLOSION_FPS);
  gExplosion->deactivate();

  SetTargetFPS(FPS);
}

void resetGame() {
  gRocket->setPosition(ROCKET_START_POS);
  gRocket->setVelocity(ROCKET_START_VELOCITY);
  gRocket->setAcceleration({0.0f, 0.0f});
  gRocket->setAngle(0.0f);
  gRocket->setThrusting(false);

  gFlame->setPosition(ROCKET_START_POS);
  gFlame->setAngle(0.0f);
  gFlame->setOpacity(0.0f);

  gMoonAngle = 0.0f;
  gMoon->setPosition(MOON_START_POS);

  for (int i = 0; i < NUMBER_OF_SATELLITES; i++) {
    gSatelliteAngles[i] = SATELLITE_START_ANGLES[i];
  }
  moveSatellites(0.0f);

  gGameStatus = PLAYING;
  gLossReason = NO_LOSS;
  gTimeAccumulator = 0.0f;

  gFuel = MAX_FUEL;

  gRocket->activate();
  gExplosion->deactivate();
}

void moveSatellites(float deltaTime) {
  for (int i = 0; i < NUMBER_OF_SATELLITES; i++) {
    gSatelliteAngles[i] += SATELLITE_ORBIT_SPEEDS[i] * deltaTime;
    gSatellites[i].setPosition(gPlanet->getPosition() +
      Vector2{cosf(gSatelliteAngles[i]), sinf(gSatelliteAngles[i])} * SATELLITE_ORBIT_RADII[i]);
  }
}

bool isTouchingAny(Entity *bodies, int bodyCount) {
  float rocketRadius = gRocket->getColliderDimensions().x / 2.0f;

  for (int i = 0; i < bodyCount; i++) {
    float distToBody = GetLength(gRocket->getPosition() - bodies[i].getPosition());
    float bodyRadius = bodies[i].getColliderDimensions().x / 2.0f;
    if (distToBody <= bodyRadius + rocketRadius) return true;
  }

  return false;
}

ContactResult checkLanding(Entity *bodies[], int bodyCount) {
  ContactResult result = NO_CONTACT;
  float rocketRadius = gRocket->getColliderDimensions().x / 2.0f;

  for (int i = 0; i < bodyCount; i++) {
    Entity *body = bodies[i];

    Vector2 outward = gRocket->getPosition() - body->getPosition();
    float distToBody = GetLength(outward);
    float bodyRadius = body->getColliderDimensions().x / 2.0f;

    // Close?
    if (distToBody > bodyRadius + rocketRadius) continue;

    // Slow?
    Vector2 relativeVelocity = gRocket->getVelocity() - body->getVelocity();
    Vector2 surfaceNormal = {outward.x / distToBody, outward.y / distToBody};
    float impactSpeed = -(relativeVelocity.x * surfaceNormal.x +
      relativeVelocity.y * surfaceNormal.y);
    float shearSpeed = fabsf(relativeVelocity.x * -surfaceNormal.y +
      relativeVelocity.y *  surfaceNormal.x);
    if (impactSpeed > SAFE_LANDING_SPEED) return CRASH;
    if (shearSpeed > SAFE_SHEAR_SPEED) return CRASH;

    // Upright?
    float uprightAngle = atan2f(outward.x, -outward.y) * RAD2DEG;
    float angleOff = fmodf(gRocket->getAngle() - uprightAngle, 360.0f);
    if (angleOff >  180.0f) angleOff -= 360.0f;
    if (angleOff < -180.0f) angleOff += 360.0f;
    if (fabsf(angleOff) > SAFE_LANDING_ANGLE) return CRASH;

    result = SAFE_LANDING;
  }

  return result;
}

void processInput() {
  if (WindowShouldClose()) gAppStatus = TERMINATED;

  if ((IsKeyDown(KEY_Q) || IsKeyDown(KEY_A)) && 
    !(IsKeyDown(KEY_E) || IsKeyDown(KEY_D))) gRotate = -1;
  else if ((IsKeyDown(KEY_E) || IsKeyDown(KEY_D)) && 
    !(IsKeyDown(KEY_Q) || IsKeyDown(KEY_A))) gRotate = 1;
  else gRotate = 0;

  if (IsKeyDown(KEY_W) || IsKeyDown(KEY_SPACE)) gIsThrusting = true;
  else gIsThrusting = false;

  if (gGameStatus != PLAYING && IsKeyPressed(KEY_R)) resetGame();
}

void update() {
  // Delta time
  float ticks = static_cast<float>(GetTime());
  float deltaTime = ticks - gPreviousTicks;
  gPreviousTicks = ticks;

  if (gExplosion->isActive()) {
    gExplosion->update(deltaTime, nullptr, 0);
    gExplosionTimer += deltaTime;
    if (gExplosionTimer >= static_cast<float>(EXPLOSION_FRAMES) / EXPLOSION_FPS) {
      gExplosion->setAnimating(false);
      gExplosion->deactivate();
    }
  }

  if (gGameStatus != PLAYING) return;

  deltaTime += gTimeAccumulator;
  if (deltaTime < FIXED_TIMESTEP) {
      gTimeAccumulator = deltaTime;
      return;
  }

  while (deltaTime >= FIXED_TIMESTEP) {
    // Moon Orbit
    gMoonAngle += MOON_ORBIT_SPEED * FIXED_TIMESTEP;
    Vector2 oldMoonPosition = gMoon->getPosition();
    gMoon->setPosition(gPlanet->getPosition() + 
      Vector2{cosf(gMoonAngle), sinf(gMoonAngle)} * MOON_ORBIT_RADIUS);
    gMoon->setVelocity((gMoon->getPosition() - oldMoonPosition) /
      FIXED_TIMESTEP);

    // Satellite Orbits
    moveSatellites(FIXED_TIMESTEP);

    // Rocket Rotation
    float dAngle = gRotate * ROTATION_SPEED * FIXED_TIMESTEP;
    gRocket->setAngle(gRocket->getAngle() + dAngle);
    Vector2 rocketFacing = {sinf(gRocket->getAngle() * DEG2RAD), -cosf(gRocket->getAngle() * DEG2RAD)};

    // Fuel
    bool isFiring = gIsThrusting && gFuel > 0.0f;
    if (isFiring) {
      gFuel -= FUEL_BURN_RATE * FIXED_TIMESTEP;
      if (gFuel < 0.0f) gFuel = 0.0f;
    }
    gRocket->setThrusting(isFiring);

    // Thrust sound
    if (isFiring) {
      if (!IsSoundPlaying(gThrustSound)) PlaySound(gThrustSound);
    } else {
      StopSound(gThrustSound);
    }

    // Rocket Acceleration
    Vector2 toPlanet = gPlanet->getPosition() - gRocket->getPosition();
    Normalise(&toPlanet);
    Vector2 gravity = toPlanet * GRAVITY;
    Vector2 thrust = (isFiring) ?
      rocketFacing * THRUST : Vector2{0.0f, 0.0f};
    Vector2 drag = gRocket->getVelocity() * (-DRAG);
    gRocket->setAcceleration(gravity + thrust + drag);

    // Update Rocket
    gRocket->update(FIXED_TIMESTEP, nullptr, 0);

    // Check Loss Conditions
    if (gRocket->getPosition().x < -ROCKET_SCALE.x * 2 ||  
      gRocket->getPosition().x > SCREEN_WIDTH + ROCKET_SCALE.x * 2 ||
      gRocket->getPosition().y < -ROCKET_SCALE.y * 2  ||  
      gRocket->getPosition().y > SCREEN_HEIGHT + ROCKET_SCALE.y * 2) {
      gGameStatus = LOST;
      gLossReason = OFF_SCREEN;
    }

    if (gGameStatus == PLAYING && isTouchingAny(gSatellites, NUMBER_OF_SATELLITES)) {
      gGameStatus = LOST;
      gLossReason = COLLISION;
    }

    gContactResult = (gGameStatus == PLAYING)
      ? checkLanding(gLandableBodies, NUMBER_OF_LANDABLE_BODIES) : NO_CONTACT;
    if (gContactResult == SAFE_LANDING) {
      gGameStatus = WON;
    } else if (gContactResult == CRASH) {
      gGameStatus = LOST;
      gLossReason = CRASH_LANDING;
    }

    if (gGameStatus != PLAYING) {
      StopSound(gThrustSound);
      if (gGameStatus == LOST && gLossReason != OFF_SCREEN) {
        PlaySound(gExplosionSound);

        // Swap the rocket for an explosion
        gExplosion->setPosition(gRocket->getPosition());
        gExplosion->resetAnimation();
        gExplosion->setAnimating(true);
        gExplosion->activate();
        gExplosionTimer = 0.0f;

        gRocket->deactivate();
        gFlame->setOpacity(0.0f);
      }
      break;
    }

    // Rocket Flame
    float flameOpacity = gFlame->getOpacity();
    if (isFiring) flameOpacity += FLAME_FADE_SPEED * FIXED_TIMESTEP;
    else flameOpacity -= FLAME_FADE_SPEED * FIXED_TIMESTEP;
    if (flameOpacity > 1.0f) flameOpacity = 1.0f;
    if (flameOpacity < 0.0f) flameOpacity = 0.0f;
    gFlame->setOpacity(flameOpacity);
    float rocketAngle = gRocket->getAngle();
    float flameDistance = (ROCKET_SCALE.y + FLAME_SCALE.y) / 2.0f - FLAME_OVERLAP;
    gFlame->setPosition({
      gRocket->getPosition().x - rocketFacing.x * flameDistance,
      gRocket->getPosition().y - rocketFacing.y * flameDistance
    });
    gFlame->setAngle(rocketAngle);

    // Update deltaTime
    deltaTime -= FIXED_TIMESTEP;
  }

  gTimeAccumulator = deltaTime;
}

void render() {
  BeginDrawing();
  ClearBackground(ColorFromHex(BG_COLOUR));

  gBackground->render();
  gPlanet->render();
  gMoon->render();
  for (int i = 0; i < NUMBER_OF_SATELLITES; i++) gSatellites[i].render();
  gFlame->render();
  gRocket->render();
  gExplosion->render();

  // HUD
  constexpr int HUD_X = 20, HUD_Y = 20, BAR_WIDTH = 200, BAR_HEIGHT = 12;
  float fuelFraction = gFuel / MAX_FUEL;
  Color fuelColor = (fuelFraction < 0.25f) ? RED : GREEN;

  DrawText(TextFormat("FUEL: %d", static_cast<int>(gFuel)), 
    HUD_X, HUD_Y, 20, WHITE);
  DrawRectangle(HUD_X, HUD_Y + 25, static_cast<int>(BAR_WIDTH * fuelFraction), 
    BAR_HEIGHT, fuelColor);
  DrawRectangleLines(HUD_X, HUD_Y + 25, BAR_WIDTH, BAR_HEIGHT, WHITE);
  
  DrawText(TextFormat("SPEED: %d", 
    static_cast<int>(GetLength(gRocket->getVelocity()))), 
    HUD_X, HUD_Y + 50, 20, WHITE);

  if (gGameStatus != PLAYING) {
    const char *title = nullptr;
    const char *hint = nullptr;
    Color titleColor = WHITE;
    if (gGameStatus == LOST) {
      title = "MISSION FAILED";
      titleColor = RED;
      if (gLossReason == OFF_SCREEN) {
        hint  = "You left the mission area - press R to restart";
      }
      if (gLossReason == CRASH_LANDING) {
        hint = "You crashed - press R to restart";
      }
      if (gLossReason == COLLISION) {
        hint = "You hit a satellite - press R to restart";
      }
    }
    else {
      title = "MISSION ACCOMPLISHED";
      hint = "You landed safely - press R to restart";
      titleColor = GREEN;
    }

    if (title != nullptr && hint != nullptr) {
      DrawText(title, (SCREEN_WIDTH - MeasureText(title, 50)) / 2, ORIGIN.y - 280, 50, titleColor);
      DrawText(hint,  (SCREEN_WIDTH - MeasureText(hint, 20)) / 2,  ORIGIN.y  - 200, 20, WHITE);
    }
  }

  EndDrawing();
}

void shutdown() {
  delete gBackground;
  delete gPlanet;
  delete gMoon;
  delete[] gSatellites; 
  delete gRocket;
  delete gFlame;
  delete gExplosion;

  UnloadSound(gThrustSound);
  UnloadSound(gExplosionSound);
  CloseAudioDevice();

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
