#include "bird.h"

Bird::Bird()
  : _y(BIRD_START_Y), _velocityNum(0), _dead(false), _wingFrame(0), _wingTick(0) {}

void Bird::reset() {
  _y           = BIRD_START_Y;
  _velocityNum = 0;
  _dead        = false;
  _wingFrame   = 0;
  _wingTick    = 0;
}

void Bird::jump() {
  if (_dead) return;
  _velocityNum = JUMP_VELOCITY * GRAVITY_DEN;
}

void Bird::update() {
  if (_dead) return;

  // Apply gravity (integer fixed-point: velocity stored *GRAVITY_DEN)
  _velocityNum += GRAVITY_NUM;

  // Clamp to terminal velocity
  if (_velocityNum > MAX_FALL_VEL * GRAVITY_DEN)
    _velocityNum = MAX_FALL_VEL * GRAVITY_DEN;

  _y += _velocityNum / GRAVITY_DEN;

  // Ceiling
  if (_y < 0) {
    _y = 0;
    _velocityNum = 0;
  }

  // Ground collision
  if (_y + BIRD_H >= GROUND_Y) {
    _y = GROUND_Y - BIRD_H;
    _dead = true;
  }

  // Wing animation (toggle every 3 ticks)
  _wingTick++;
  if (_wingTick >= 3) {
    _wingTick  = 0;
    _wingFrame = (_wingFrame + 1) % 3;
  }
}

int  Bird::getY() const        { return _y; }
int  Bird::getVelocity() const { return _velocityNum / GRAVITY_DEN; }
bool Bird::isDead() const      { return _dead; }
void Bird::kill()              { _dead = true; }
