#include "pipe.h"

Pipe::Pipe()
  : _x(OLED_WIDTH + PIPE_W), _gapY(20), _gapSize(PIPE_GAP),
    _active(false), _scored(false) {}

void Pipe::reset(int startX, int gapY, int gapSize) {
  _x        = startX;
  _gapY     = gapY;
  _gapSize  = gapSize;
  _active   = true;
  _scored   = false;
}

void Pipe::update() {
  if (!_active) return;
  _x -= PIPE_SPEED;
  if (_x + PIPE_W < 0) _active = false;
}

bool Pipe::isActive()    const { return _active; }
bool Pipe::isOffScreen() const { return _x + PIPE_W < 0; }
void Pipe::deactivate()        { _active = false; }

bool Pipe::birdPassed(int birdX) const {
  // Bird's right edge just passed pipe's right edge
  if (_scored) return false;
  return (birdX > _x + PIPE_W);
}

bool Pipe::collidesWith(int bx, int by, int bw, int bh) const {
  if (!_active) return false;

  // Check horizontal overlap
  if (bx + bw <= _x || bx >= _x + PIPE_W) return false;

  // Check vertical: collision if bird is in top pipe or bottom pipe
  bool inTopPipe    = (by < _gapY);
  bool inBottomPipe = (by + bh > _gapY + _gapSize);

  return inTopPipe || inBottomPipe;
}
