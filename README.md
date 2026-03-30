# 🐦 Flappy Bird — ESP8266 + OLED

> A full Flappy Bird clone running on an ESP8266 microcontroller with a 0.96" OLED display. One button. Animated wings. Scrolling ground. Increasing difficulty. High score tracking.

No WiFi. No internet. Just you, a tiny screen, and your reflexes.

---

## Features

| Feature | Details |
|---|---|
| 🐦 Animated Bird | 3-frame wing animation (up / mid / down) |
| 🌿 Pipes | Randomised gap positions every run |
| 📈 Difficulty | Gap narrows every 5 points (down to minimum) |
| 🏆 High Score | Persists across rounds until power off |
| 🌟 New Record | Special screen when you beat your best |
| 🎬 Death Flash | Brief white flash on collision |
| 🌍 Scrolling Ground | Animated ground dashes synced to pipe speed |
| 🖥️ Title Screen | Proper title with bird graphic |

---

## Hardware

### Components

| Part | Details |
|---|---|
| Board | ESP8266 - NodeMCU or Wemos D1 Mini |
| Display | 0.96" OLED, SSD1306, 128×64 px, I2C |
| Button | Any momentary push button (NO type) |
| Power | Micro-USB 5V |

### Wiring

| ESP8266 Pin | Connected To | Notes |
|---|---|---|
| D1 (GPIO5) | OLED SCL | I2C Clock |
| D2 (GPIO4) | OLED SDA | I2C Data |
| 3V3 | OLED VCC | Power (3.3V only!) |
| GND | OLED GND + Button | Ground |
| D3 (GPIO0) | Button → GND | Active LOW input |

> **No external button needed to test!** GPIO0 is the built-in FLASH button on NodeMCU and D1 Mini boards.

---

## Project Structure

```
ESP8266-FlappyBird-OLED/
├── main.ino          ← Game loop + state machine
├── config.h          ← All tunable constants
├── bird.h / bird.cpp ← Physics + wing animation
├── pipe.h / pipe.cpp ← Pipe movement + collision
├── renderer.h        ← Drawing interface
├── renderer.cpp      ← All OLED rendering
└── README.md
```

---

## Library Installation

**Sketch → Include Library → Manage Libraries**, install:

| Library | Author |
|---|---|
| Adafruit GFX | Adafruit |
| Adafruit SSD1306 | Adafruit |

No other external libraries needed.

---

## Flash & Play

1. Open `main.ino` in Arduino IDE
2. **Tools → Board** → `NodeMCU 1.0` or `LOLIN(Wemos) D1 Mini`
3. **Tools → Port** → select your COM port
4. **Upload**
5. Press the button to start

---

## How to Play

| Screen | Button action |
|---|---|
| Title screen | Press → start game |
| Playing | Press → bird jumps |
| Game over | Press → retry immediately |

- The bird falls continuously due to gravity
- Each press gives an upward flap
- Fly through the gaps between pipes
- Every pipe cleared = +1 point
- Gap gets smaller every 5 points — good luck

---

## Screens

### Title
```
┌──────────────────────────┐
│       FLAPPY             │
│        BIRD         🐦   │
│                          │
│    Press to start        │
└──────────────────────────┘
```

### Playing
```
┌──────────────────────────┐
│           5              │
│   ████                   │
│   █  █                   │
│ 🐦  █  █                 │
│   ████                   │
│__ __ __ __ __ __ __ __ _ │
└──────────────────────────┘
```

### Game Over
```
┌──────────────────────────┐
│        GAME OVER         │
│  ────────────────────    │
│      Score:  12          │
│      Best:   17          │
│       [dead bird]        │
│     Press to retry       │
└──────────────────────────┘
```

### New Record
```
┌──────────────────────────┐
│        GAME OVER         │
│  ────────────────────    │
│     * NEW BEST! *        │
│      Score:  18          │
│      Best:   18          │
│     Press to retry       │
└──────────────────────────┘
```

---

## Configuration

Edit `config.h` to tune the game:

```cpp
// Game speed — lower = faster (default 35ms ≈ 28fps)
#define GAME_TICK_MS      35

// Pipe movement speed (pixels per tick)
#define PIPE_SPEED        2

// Starting gap between pipes (px)
#define PIPE_GAP          20

// Minimum gap at max difficulty
#define PIPE_GAP_MIN      13

// Points needed before gap shrinks by 1
#define DIFFICULTY_STEP   5

// Jump strength (more negative = higher jump)
#define JUMP_VELOCITY    -6

// Gravity (applied each tick as fraction GRAVITY_NUM/GRAVITY_DEN)
#define GRAVITY_NUM       3
#define GRAVITY_DEN       2
```

### Difficulty Presets

| Mode | GAME_TICK_MS | PIPE_SPEED | PIPE_GAP | PIPE_GAP_MIN |
|---|---|---|---|---|
| Easy | 45 | 2 | 24 | 16 |
| Normal (default) | 35 | 2 | 20 | 13 |
| Hard | 30 | 3 | 18 | 12 |
| Insane | 25 | 3 | 16 | 10 |

---

## Architecture

```
main.ino
│
├── STATE_TITLE    → wait for button → STATE_PLAYING
│
├── STATE_PLAYING  → each tick:
│   ├── bird.update()       (gravity + ground collision)
│   ├── pipe[i].update()    (move left)
│   ├── check score         (bird passed pipe)
│   ├── check collision     → STATE_FLASH
│   └── renderer.drawPlaying()
│
├── STATE_FLASH    → white screen for 600ms → STATE_GAMEOVER
│
└── STATE_GAMEOVER → wait for button → STATE_PLAYING
```

---

## Troubleshooting

**Display blank**
- Check SDA → D2 (GPIO4), SCL → D1 (GPIO5)
- Use 3.3V for OLED power — not 5V
- Confirm I2C address is `0x3C` in `config.h`

**Bird falls immediately / feels wrong**
- Adjust `GRAVITY_NUM` and `JUMP_VELOCITY` in `config.h`
- Lower `GRAVITY_NUM` = floatier bird

**Pipes too hard / easy**
- Increase `PIPE_GAP` for more room
- Increase `GAME_TICK_MS` to slow down

**Button not responding**
- GPIO0 is active LOW with internal pull-up
- Button should bridge GPIO0 to GND
- Open Serial Monitor at 115200 baud to verify input

---

## License

MIT — free to use, modify, and build on.

---

## Credits

Inspired by the original Flappy Bird by Dong Nguyen.  
Built with ESP8266 Arduino core + Adafruit SSD1306.
