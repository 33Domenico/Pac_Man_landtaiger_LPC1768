# Pac-Man for LandTiger Board

## Project Overview
This project consists of implementing a **Pac-Man** game for the **LandTiger Board**, using **Keil μVision** in the **SW_Debug** environment. The purpose is to gain confidence in software debugging for the LPC1768 microcontroller while developing a functional game.

### Delivery Information
- **Delivery Date:** 12th January 2025
- **Extra Points:** Up to 2 points (Christmas Edition)
- **Expected Delivery:**
  - `extrapoint1.zip`: Zipped project folder containing all files with saved compilation options.
  - This document compiled in PDF format.

## Game Description
Pac-Man is a classic maze arcade game where the player navigates a maze, eating pills and avoiding obstacles. This implementation follows a simplified version of the original game.

### LandTiger Board Implementation
- **Emulation:** Use the **LandTiger emulator** (`SW_Debug`).
- **Physical Board Option:** If using a real board, adjust Pac-Man's movement speed accordingly.
- **Code Documentation:** Main variables and game logic are in `sample.c`, with comments for better understanding.

## Game Specifications

### Display and UI
- The **LandTiger display** renders the **maze** with:
  - **240 Standard Pills**.
  - **6 Power Pills** placed at random positions and times.
  - A **central box** (potential future use).
  - **Score counter**, **Remaining Lives**, and a **Countdown Timer**.

### Gameplay Mechanics
1. **Pills Placement**
   - The maze is filled with **240 Standard Pills**.
   - **6 Power Pills** appear randomly.

2. **Pac-Man Movement**
   - Controlled using the **joystick**.
   - Moves in the chosen direction until:
     - The player selects a new direction.
     - Pac-Man hits a wall (stops and waits for input).
   - Speed is adjustable for smooth gameplay.

3. **Teleportation Mechanic**
   - When Pac-Man reaches a **central teleport location**, he exits from one side and re-enters from the opposite side, maintaining the same direction.

4. **Scoring System**
   - Eating a **Standard Pill** increases the score by **10 points**.
   - Eating a **Power Pill** increases the score by **50 points**.
   - Every **1000 points**, Pac-Man earns **one extra life** (starting with one life).

5. **Game States**
   - **Pause Mode:** Pressing **INT0** pauses the game ("PAUSE" message displayed). Press again to resume.
   - **Countdown Timer:** Starts from **60 seconds** and counts down to **0**.
   - **Win Condition:** If all pills are eaten before time runs out, a **"Victory!"** screen appears.
   - **Lose Condition:** If the timer reaches **0** before all pills are eaten, a **"Game Over!"** screen appears.

## Emulator Configuration
- **Ensure that the project is saved with all compilation options.**
- **Attach relevant comments and settings.**

## Submission Guidelines
- Compress all necessary files into **`extrapoint1.zip`**.
- Include **this document as a PDF**.
- Submit a **4-minute explanatory video** demonstrating the project and debugging process.


