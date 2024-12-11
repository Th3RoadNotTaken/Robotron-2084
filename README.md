# Robotron-2084

<img width="946" alt="Robotron_StartScreen" src="https://github.com/user-attachments/assets/21a5a278-541f-4a6b-badf-5d32554543e4" />

This is a clone of the iconic twin-stick shooter Robotron 2084, built using C and OpenGL. 

## Steps to play:
- Clone the project
- Open the Game folder and launch the exe

## Below are the key technical implementations:
## Object-Oriented Structure
- Designed an object-oriented architecture in C using vtables, function pointers, and callbacks.
- Developed a BaseObject class with a vtable for Draw and Update functions and an ObjectManager class for registering and deregistering objects dynamically.
- Objects could seamlessly be created, updated, and destroyed, ensuring a modular and extensible design.
  
## Object Pooling
- Implemented object pooling to handle the game's numerous chaotic entities efficiently.
- Created object pools for enemies, bullets, obstacles, and projectiles, with managers overseeing creation, recycling, and deletion.
- Example: Enemies respawn at new locations if the player restarts a level, and destroyed enemies are recycled to save memory.

## Sprite Animations
- Built a SpriteAnimation system with structs for SpriteFrames and SpriteAnimations, each containing frame durations and relevant metadata.
- Added 8-directional animations for players, enemies, and obstacles.
- Included explosion effects for visual enhancement, synchronizing sprite frames with events like enemy deaths.
  
## Vertex Coloring and Effects
- Utilized vertex coloring via OpenGL blend modes to mimic the original game’s vibrant effects.
- Implemented dynamic strobing effects by applying color variations to sprites on each frame.
- Designed a level transition effect using expanding rectangles with color palette shifts, simulating the original game’s visuals.

## Enemy AI
- Recreated various enemy types with unique behaviors:
- Grunts: Follow the player diagonally or in straight lines based on relative position.
- Spheroids/Quarks: Move sinusoidally with differing amplitudes and oscillation rates, selecting random waypoints.
- AI behaviors emulate the chaotic challenge of the original game.

## Collision System
- Implemented collision detection using an Axis-Aligned Bounding Box (AABB) for efficiency.
- Created a collision system with a vtable-based approach. Collisions are handled between object types such as:
  - Bullet ↔ Enemy
  - Player ↔ Enemy
  - Player ↔ Obstacle
- Each object type defines its collision behavior, enabling reusable and type-specific logic.
  
## Wave Spawning and Level Design
- Designed wave spawning logic to progressively increase difficulty:
- Enemy and obstacle types spawn with specific probabilities and scaling parameters.
- Expanded enemy counts and adjusted spawn probabilities in subsequent levels.
- Leveraged the pooling system for memory-efficient spawning and recycling of objects.

## High Score System
- Created a persistent high-score system:
- Reads scores from a file, updates them based on the player’s performance, and writes back updated scores.
- Ensures high scores are retained across game sessions.
  
This project showcases advanced C programming techniques, efficient memory management, and OpenGL-based graphics rendering to recreate a classic arcade game.

Feel free to check out the source code!
