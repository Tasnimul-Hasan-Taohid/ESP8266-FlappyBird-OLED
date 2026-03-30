#pragma once

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "bird.h"
#include "pipe.h"

class Renderer {
public:
  Renderer();
  bool begin();

  void drawTitle();
  void drawPlaying(const Bird& bird, Pipe pipes[], int numPipes,
                   int score, int groundOffset);
  void drawFlash();
  void drawGameOver(int score, int highScore, bool newRecord);

private:
  Adafruit_SSD1306 _display;

  void _drawBird(const Bird& bird);
  void _drawPipe(const Pipe& pipe);
  void _drawGround(int offset);
  void _drawScore(int score);
  void _drawCentered(const String& text, int y, uint8_t size = 1);
};
