#pragma once
#include <Arduino.h>
#include "config.h"

class Pipe {
public:
  Pipe();
  void reset(int startX, int gapY, int gapSize);
  void update();                    // Move left each tick

  bool isActive() const;
  bool isOffScreen() const;
  void deactivate();

  bool collidesWith(int bx, int by, int bw, int bh) const;
  bool birdPassed(int birdX) const; // true the tick bird crosses right edge

  int getX()      const { return _x; }
  int getGapY()   const { return _gapY; }
  int getGapSize()const { return _gapSize; }

private:
  int  _x;
  int  _gapY;      // Y where the gap starts
  int  _gapSize;
  bool _active;
  bool _scored;    // So we only award score once
};
