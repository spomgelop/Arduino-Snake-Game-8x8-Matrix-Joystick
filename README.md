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
- [LedControl by Eberhard Fahle](https://github.com/wayoda/LedControl)

# How to use
- Install the LedControl library using the Arduino IDE library manager
- Upload the code to your Arduino
- The 8x8 matrix should display a 2x2 square in the middle
- Press in on the joystick to start the game
- Move the joystick in the direction you want the snake to move
- Eat the food that randomly pops up on the 8x8 matrix display
- Avoid collision with the snake or the 4 walls
- The snake will grow longer the more you eat the food
- If a collision appears the LEDs on the matrix display will flash 3 times
- Press the joystick to play again

# Simulation
[Wokwi Simulation](https://wokwi.com/projects/471003872741771265)

[Video Demonstration](https://www.youtube.com/shorts/27aSPxvOeAM)

<img width="757" height="677" alt="image" src="https://github.com/user-attachments/assets/8dc85708-f98d-4502-8ac0-b98a21a45954" />

