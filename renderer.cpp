#include "renderer.h"

Renderer::Renderer()
  : _display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET) {}

bool Renderer::begin() {
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!_display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) {
    Serial.println("[RENDERER] Display init failed");
    return false;
  }
  _display.clearDisplay();
  _display.setTextColor(SSD1306_WHITE);
  _display.display();
  Serial.println("[RENDERER] OK");
  return true;
}

// ─── Title Screen ──────────────────────────────────────────────
void Renderer::drawTitle() {
  _display.clearDisplay();

  // Big title
  _drawCentered("FLAPPY", 4, 2);
  _drawCentered("BIRD", 22, 2);

  // Decorative bird on right
  // Body
  _display.fillRect(100, 28, 12, 10, SSD1306_WHITE);
  // Eye
  _display.fillCircle(109, 30, 2, SSD1306_WHITE);
  _display.fillCircle(110, 30, 1, SSD1306_BLACK);
  // Beak
  _display.fillRect(112, 31, 4, 3, SSD1306_WHITE);
  // Wing (mid position)
  _display.fillRect(98, 33, 5, 4, SSD1306_WHITE);

  // Ground line
  _display.drawFastHLine(0, GROUND_Y, OLED_WIDTH, SSD1306_WHITE);

  // Prompt
  _drawCentered("Press to start", 57);

  _display.display();
}

// ─── Game Screen ───────────────────────────────────────────────
void Renderer::drawPlaying(const Bird& bird, Pipe pipes[], int numPipes,
                            int score, int groundOffset) {
  _display.clearDisplay();

  // Pipes (behind bird)
  for (int i = 0; i < numPipes; i++) {
    if (pipes[i].isActive()) _drawPipe(pipes[i]);
  }

  // Bird
  _drawBird(bird);

  // Ground
  _drawGround(groundOffset);

  // Score
  _drawScore(score);

  _display.display();
}

// ─── Flash Screen (death) ──────────────────────────────────────
void Renderer::drawFlash() {
  _display.fillScreen(SSD1306_WHITE);
  _display.display();
}

// ─── Game Over Screen ──────────────────────────────────────────
void Renderer::drawGameOver(int score, int highScore, bool newRecord) {
  _display.clearDisplay();

  _drawCentered("GAME OVER", 2, 1);
  _display.drawFastHLine(20, 12, 88, SSD1306_WHITE);

  if (newRecord) {
    _drawCentered("* NEW BEST! *", 16);
  }

  String scoreStr = "Score:  " + String(score);
  String bestStr  = "Best:   " + String(highScore);
  _drawCentered(scoreStr, newRecord ? 28 : 22);
  _drawCentered(bestStr,  newRecord ? 38 : 32);

  // Dead bird graphic
  // Body (upside down tilt)
  _display.fillRect(57, 42, 12, 9, SSD1306_WHITE);
  // Eye X (dead)
  _display.drawLine(64, 44, 66, 46, SSD1306_BLACK);
  _display.drawLine(66, 44, 64, 46, SSD1306_BLACK);
  // Beak
  _display.fillRect(69, 44, 4, 3, SSD1306_WHITE);
  // Wing drooping down
  _display.fillRect(55, 47, 5, 4, SSD1306_WHITE);

  _display.drawFastHLine(0, GROUND_Y, OLED_WIDTH, SSD1306_WHITE);
  _drawCentered("Press to retry", 57);

  _display.display();
}

// ─── Private Draw Helpers ──────────────────────────────────────

void Renderer::_drawBird(const Bird& bird) {
  int x = BIRD_X;
  int y = bird.getY();
  int f = bird.getWingFrame();

  // Body (10x8)
  _display.fillRect(x, y + 1, 10, 6, SSD1306_WHITE);
  // Head bump
  _display.fillRect(x + 2, y, 6, 2, SSD1306_WHITE);
  // Tail
  _display.fillPixel(x, y + 7, SSD1306_WHITE);

  // Eye
  _display.fillCircle(x + 7, y + 3, 2, SSD1306_WHITE);
  _display.fillCircle(x + 8, y + 3, 1, SSD1306_BLACK); // pupil

  // Beak
  _display.fillRect(x + 10, y + 3, 3, 2, SSD1306_WHITE);

  // Wing — 3 frames: up / mid / down
  switch (f) {
    case 0: // wing up
      _display.fillRect(x + 2, y - 2, 5, 3, SSD1306_WHITE);
      break;
    case 1: // wing mid
      _display.fillRect(x + 1, y + 2, 5, 3, SSD1306_WHITE);
      break;
    case 2: // wing down
      _display.fillRect(x + 2, y + 6, 5, 3, SSD1306_WHITE);
      break;
  }
}

void Renderer::_drawPipe(const Pipe& pipe) {
  int x    = pipe.getX();
  int gapY = pipe.getGapY();
  int gap  = pipe.getGapSize();

  // Top pipe body
  if (gapY > 0) {
    _display.fillRect(x, 0, PIPE_W, gapY, SSD1306_WHITE);
    // Cap (slightly wider)
    _display.fillRect(x - 1, gapY - 4, PIPE_W + 2, 4, SSD1306_WHITE);
    // Inner shadow line
    _display.drawFastVLine(x + 2, 0, gapY - 4, SSD1306_BLACK);
  }

  // Bottom pipe body
  int botTop = gapY + gap;
  if (botTop < GROUND_Y) {
    _display.fillRect(x, botTop, PIPE_W, GROUND_Y - botTop, SSD1306_WHITE);
    // Cap
    _display.fillRect(x - 1, botTop, PIPE_W + 2, 4, SSD1306_WHITE);
    // Inner shadow line
    _display.drawFastVLine(x + 2, botTop + 4, GROUND_Y - botTop - 4, SSD1306_BLACK);
  }
}

void Renderer::_drawGround(int offset) {
  _display.drawFastHLine(0, GROUND_Y, OLED_WIDTH, SSD1306_WHITE);

  // Scrolling dashes below the line
  int dashOffset = offset % (GROUND_SEGMENT * 2);
  for (int x = -dashOffset; x < OLED_WIDTH; x += GROUND_SEGMENT * 2) {
    _display.drawFastHLine(x, GROUND_Y + 2, GROUND_SEGMENT, SSD1306_WHITE);
  }
}

void Renderer::_drawScore(int score) {
  _display.setTextSize(1);
  String s = String(score);
  // Outlined score in top-center for visibility
  int x = (OLED_WIDTH - s.length() * 6) / 2;
  // Shadow
  _display.setTextColor(SSD1306_BLACK);
  _display.setCursor(x + 1, 3);
  _display.print(s);
  // White text
  _display.setTextColor(SSD1306_WHITE);
  _display.setCursor(x, 2);
  _display.print(s);
}

void Renderer::_drawCentered(const String& text, int y, uint8_t size) {
  _display.setTextSize(size);
  int16_t x1, y1;
  uint16_t w, h;
  _display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  int x = max(0, (int)((OLED_WIDTH - (int)w) / 2));
  _display.setCursor(x, y);
  _display.setTextColor(SSD1306_WHITE);
  _display.print(text);
}
