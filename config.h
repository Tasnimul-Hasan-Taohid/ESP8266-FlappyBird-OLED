#pragma once

// ─── OLED ───────────────────────────────────────────────────────
#define OLED_SDA          4        // D2
#define OLED_SCL          5        // D1
#define OLED_WIDTH        128
#define OLED_HEIGHT       64
#define OLED_RESET        -1
#define OLED_I2C_ADDR     0x3C

// ─── Button ─────────────────────────────────────────────────────
#define BUTTON_PIN        0        // D3 / GPIO0 (FLASH button, active LOW)
#define DEBOUNCE_MS       40

// ─── Display Zones ──────────────────────────────────────────────
#define GROUND_Y          56       // Y position of ground line
#define SKY_Y             0

// ─── Bird ───────────────────────────────────────────────────────
#define BIRD_X            20       // Fixed horizontal position
#define BIRD_W            10
#define BIRD_H            8
#define BIRD_START_Y      24
#define GRAVITY_NUM       3        // Gravity as integer fraction: vel += GRAVITY_NUM / GRAVITY_DEN
#define GRAVITY_DEN       2        // per tick
#define JUMP_VELOCITY    -6        // Upward velocity on jump (negative = up)
#define MAX_FALL_VEL      8        // Terminal velocity

// ─── Pipes ──────────────────────────────────────────────────────
#define PIPE_W            10
#define PIPE_GAP          20       // Vertical gap between top and bottom pipe
#define PIPE_SPACING      64       // Horizontal distance between pipes
#define PIPE_SPEED        2        // Pixels per tick
#define PIPE_MIN_TOP      6        // Min height of top pipe
#define PIPE_MAX_TOP      34       // Max height of top pipe (leaves room for gap + bottom)
#define NUM_PIPES         2        // Active pipes on screen simultaneously

// ─── Scrolling Ground ───────────────────────────────────────────
#define GROUND_SCROLL_SPEED  2     // Matches pipe speed
#define GROUND_SEGMENT       8     // Width of each ground dash

// ─── Game Timing ────────────────────────────────────────────────
#define GAME_TICK_MS      35       // ~28 fps
#define FLASH_DURATION_MS 600      // Screen flash on death

// ─── Difficulty ─────────────────────────────────────────────────
// Every DIFFICULTY_STEP points, gap shrinks by 1 (min PIPE_GAP_MIN)
#define DIFFICULTY_STEP   5
#define PIPE_GAP_MIN      13

// ─── Screens ────────────────────────────────────────────────────
enum GameState {
  STATE_TITLE,
  STATE_PLAYING,
  STATE_FLASH,      // Brief flash on collision
  STATE_GAMEOVER
};
