#include "config.h"
#include "bird.h"
#include "pipe.h"
#include "renderer.h"

// ─── Objects ───────────────────────────────────────────────────
Bird     bird;
Pipe     pipes[NUM_PIPES];
Renderer renderer;

// ─── State ─────────────────────────────────────────────────────
GameState state        = STATE_TITLE;
int       score        = 0;
int       highScore    = 0;
bool      newRecord    = false;

// ─── Timing ────────────────────────────────────────────────────
unsigned long lastTick     = 0;
unsigned long flashStart   = 0;

// ─── Ground scroll ─────────────────────────────────────────────
int groundOffset = 0;

// ─── Button ────────────────────────────────────────────────────
bool lastBtnState   = HIGH;
bool btnPressed     = false;
unsigned long lastDebounce = 0;

// ─── Pipe spawning ─────────────────────────────────────────────
int nextPipeX = 0;   // X position where next pipe will spawn

// ─── Helpers ───────────────────────────────────────────────────
void readButton() {
  bool raw = digitalRead(BUTTON_PIN);
  unsigned long now = millis();
  if (raw != lastBtnState) lastDebounce = now;
  if ((now - lastDebounce) > DEBOUNCE_MS) {
    if (lastBtnState == HIGH && raw == LOW) {
      btnPressed = true;
    }
  }
  lastBtnState = raw;
}

bool consumePress() {
  if (btnPressed) { btnPressed = false; return true; }
  return false;
}

int currentGap() {
  // Gap shrinks every DIFFICULTY_STEP points
  int reduction = score / DIFFICULTY_STEP;
  int gap = PIPE_GAP - reduction;
  return max(gap, PIPE_GAP_MIN);
}

int randomGapY() {
  // Use millis() lower bits for pseudo-random, clamped to safe range
  return PIPE_MIN_TOP + (int)(millis() % (PIPE_MAX_TOP - PIPE_MIN_TOP + 1));
}

void spawnPipe(int x) {
  int gap  = currentGap();
  int gapY = randomGapY();
  // Make sure bottom pipe doesn't go below ground
  if (gapY + gap > GROUND_Y - 4) gapY = GROUND_Y - 4 - gap;
  if (gapY < PIPE_MIN_TOP)        gapY = PIPE_MIN_TOP;

  for (int i = 0; i < NUM_PIPES; i++) {
    if (!pipes[i].isActive()) {
      pipes[i].reset(x, gapY, gap);
      Serial.print("[PIPE] Spawned at x=");
      Serial.print(x);
      Serial.print(" gapY=");
      Serial.print(gapY);
      Serial.print(" gap=");
      Serial.println(gap);
      return;
    }
  }
}

void initPipes() {
  for (int i = 0; i < NUM_PIPES; i++) pipes[i].deactivate();
  // First pipe appears a bit after the right edge
  spawnPipe(OLED_WIDTH + 20);
  nextPipeX = OLED_WIDTH + 20 + PIPE_SPACING;
}

void setState(GameState s) {
  state = s;
  const char* names[] = {"TITLE","PLAYING","FLASH","GAMEOVER"};
  Serial.print("[STATE] -> ");
  Serial.println(names[(int)s]);
}

// ──────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(DEBUG_BAUD);
  delay(100);
  Serial.println("\n\n== ESP8266 Flappy Bird ==");

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  if (!renderer.begin()) {
    Serial.println("[SETUP] Display FATAL");
    while (1) yield();
  }

  renderer.drawTitle();
  Serial.println("[SETUP] Ready");
}

// ──────────────────────────────────────────────────────────────
void loop() {
  readButton();
  unsigned long now = millis();

  switch (state) {

    // ── TITLE ─────────────────────────────────────────────────
    case STATE_TITLE: {
      if (consumePress()) {
        // Start game
        bird.reset();
        initPipes();
        score       = 0;
        newRecord   = false;
        groundOffset = 0;
        lastTick    = now;
        setState(STATE_PLAYING);
        bird.jump(); // immediate first jump so player isn't confused
      }
      break;
    }

    // ── PLAYING ───────────────────────────────────────────────
    case STATE_PLAYING: {
      // Jump on press
      if (consumePress()) {
        bird.jump();
      }

      // Game tick
      if (now - lastTick >= GAME_TICK_MS) {
        lastTick = now;

        // Update bird
        bird.update();

        // Update pipes + score
        for (int i = 0; i < NUM_PIPES; i++) {
          if (!pipes[i].isActive()) continue;

          // Score: bird just cleared the pipe
          if (pipes[i].birdPassed(bird.right())) {
            score++;
            Serial.print("[GAME] Score: ");
            Serial.println(score);
          }

          // Collision
          if (pipes[i].collidesWith(bird.left(), bird.top(), BIRD_W, BIRD_H)) {
            bird.kill();
          }

          pipes[i].update();
        }

        // Spawn next pipe when the rightmost active pipe has moved enough
        // Check if any pipe just crossed nextPipeX threshold
        bool spawnNeeded = true;
        for (int i = 0; i < NUM_PIPES; i++) {
          if (pipes[i].isActive() && pipes[i].getX() >= nextPipeX - PIPE_SPEED) {
            spawnNeeded = false;
          }
        }
        // Also check if no pipes are active (edge case)
        bool anyActive = false;
        for (int i = 0; i < NUM_PIPES; i++) if (pipes[i].isActive()) anyActive = true;

        if (!anyActive || spawnNeeded) {
          spawnPipe(nextPipeX);
          nextPipeX += PIPE_SPACING;
        }

        // Scroll ground
        groundOffset = (groundOffset + GROUND_SCROLL_SPEED) % (GROUND_SEGMENT * 2);

        // Check death
        if (bird.isDead()) {
          flashStart = now;
          renderer.drawFlash();
          setState(STATE_FLASH);
          break;
        }

        // Draw frame
        renderer.drawPlaying(bird, pipes, NUM_PIPES, score, groundOffset);
      }
      break;
    }

    // ── FLASH (brief white flash on death) ─────────────────────
    case STATE_FLASH: {
      if (now - flashStart >= FLASH_DURATION_MS) {
        // Finalize score
        if (score > highScore) {
          highScore = score;
          newRecord = true;
        }
        renderer.drawGameOver(score, highScore, newRecord);
        setState(STATE_GAMEOVER);
      }
      break;
    }

    // ── GAME OVER ──────────────────────────────────────────────
    case STATE_GAMEOVER: {
      if (consumePress()) {
        // Restart
        bird.reset();
        initPipes();
        score        = 0;
        newRecord    = false;
        groundOffset = 0;
        lastTick     = now;
        setState(STATE_PLAYING);
        bird.jump();
      }
      break;
    }
  }

  yield();
}
