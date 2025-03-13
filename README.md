# Door Knock Pattern Matcher

A microcontroller-based device that detects and matches knocking patterns on a door using a piezoelectric sensor and four LEDs for feedback. The system can recognize a knocking sequence even if the timing is not exact, allowing for slight variations in speed.

## Features

- Detects knocks using a piezoelectric sensor
- Compares recorded knock patterns with a stored pattern
- Allows reprogramming of the knock pattern using a button
- Provides LED feedback:
  - **Green LED**: Pattern matched successfully
  - **Red LED**: Pattern mismatch
  - **Yellow LED**: Programming mode
  - **Blue LED**: Knock detected
- Detects door opening with a reed switch

## Components Used

- Microcontroller (ESP32/Arduino)
- Piezoelectric sensor (for knock detection)
- LEDs (Green, Red, Blue, Yellow)
- Reed switch (for detecting door open state)
- Button (for programming mode)

## Wiring

| Component      | Pin |
| -------------- | --- |
| Piezo Sensor   | 36  |
| Green LED      | 12  |
| Red LED        | 27  |
| Blue LED       | 26  |
| Yellow LED     | 25  |
| Reed Switch    | 16  |
| Program Button | 32  |

## How It Works

1. **Listening for Knocks**
   - The piezoelectric sensor detects vibrations from knocking.
   - When a knock is detected, the program starts recording the time intervals between knocks.
2. **Pattern Matching**
   - The system compares the recorded knock sequence with the stored default pattern.
   - The match does not have to be exact; the system allows for variations in speed.
   - If the pattern matches, the **green LED** lights up.
   - If it does not match, the **red LED** lights up.
3. **Reprogramming the Knock Pattern**
   - Press the **program button** to enter programming mode.
   - Knock the new sequence.
   - The **yellow LED** blinks to indicate the new pattern is being recorded.
   - The new pattern is saved for future matches.
4. **Door Open Detection**
   - If the reed switch detects that the door is open, a message is printed to the serial monitor.

## Improving Sensitivity

- One way to increase the sensitivity of the piezoelectric sensor is to place a small weight on top of it, improving its response to vibrations.

## Installation

1. Flash the code onto an ESP32/Arduino board.
2. Wire the components as per the table above.
3. Power the device and test the knock detection.
4. Refer to the included schematic for wiring details.

