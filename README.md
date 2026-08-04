# Arduino-Snake-Game-8x8-Matrix-Joystick
A single-button Snake game for Arduino Uno, rendered on a MAX7219 8x8 LED matrix and controlled with an analog joystick.

# Description
In this project, I used a MAX7219 LED matrix to build a classic game called Snake. The player starts as a 3-unit-long snake and must eat food that randomly spawns on the matrix to grow longer, while avoiding the walls and the snake's own body. The game runs on a wait/play/game-over state machine and uses joystick-based directional input, with dead-zone tuning to prevent the snake from twitching or changing direction when the joystick isn't being touched.

## Components
- Arduino Uno
- MAX7219 8x8 Matrix Display
- Full Breadboard and jumper wires
- Joystick module

# Pin Connections
## MAX7219 to Arduino
- VCC → 5V (Breadboard power rail)
- GND → GND (Breadboard power rail)
- DIN → 11 (Arduino Digital)
- CS → 10 (Arduino Digital)
- CLK → 13 (Arduino Digital)

## Joystick to Arduino
- 5V → 5V (Breadboard power rail)
- GND → GND (Breadboard power rail)
- VRx → A0 (Arduino Analog)
- VRy → A1 (Arduino Analog)
- SW → 2 (Arduino Digital)

# Libraries used
- [LedControl by Eberhard Fahle](github.com/wayoda/LedControl)
