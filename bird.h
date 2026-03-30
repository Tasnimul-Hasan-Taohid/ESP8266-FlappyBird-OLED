#pragma once
#include <Arduino.h>
#include "config.h"

class Bird {
public:
  Bird();
  void reset();
  void jump();
  void update();        // Apply gravity each tick

  int  getY() const;
  int  getVelocity() const;
  bool isDead() const;
  void kill();

  // Bounding box for collision
  int left()   const { return BIRD_X; }
  int right()  const { return BIRD_X + BIRD_W; }
  int top()    const { return _y; }
  int bottom() const { return _y + BIRD_H; }

  int getWingFrame() const { return _wingFrame; }

private:
  int  _y;
  int  _velocityNum;   // Stored as integer * GRAVITY_DEN to avoid floats
  bool _dead;
  int  _wingFrame;
  int  _wingTick;
};
