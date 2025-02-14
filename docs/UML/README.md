# UML Diagrams for Collision Detection System

## Required Diagrams

1. **Class Diagram** (`class_diagram.mdj`)
   - Shows the class hierarchy and relationships
   - Includes all properties and methods
   - Shows inheritance between PhysicsObject and its derived classes

2. **Sequence Diagram** (`sequence_diagram.mdj`)
   - Shows the collision detection and resolution flow
   - Includes object interactions during physics updates
   - Demonstrates the main game loop sequence

3. **Activity Diagram** (`activity_diagram.mdj`)
   - Shows the main program flow
   - Details collision detection algorithm steps
   - Includes physics update cycle

## How to Use These Diagrams

1. Install StarUML from: https://staruml.io/
2. Open each .mdj file in StarUML
3. The diagrams are organized in separate files for clarity
4. Export diagrams as images for documentation

## Class Diagram Structure
```
PhysicsObject (Abstract)
├── Properties
│   ├── position: vec2
│   ├── velocity: vec2
│   ├── acceleration: vec2
│   ├── mass: float
│   ├── restitution: float
│   └── isStatic: bool
├── Methods
│   ├── update(deltaTime: float)
│   ├── checkCollision(other: PhysicsObject): bool
│   └── draw(): void

Circle
├── Properties
│   ├── radius: float
│   └── color: vec3
└── Methods
    ├── checkCollision(other: PhysicsObject): bool
    └── draw(): void

Rectangle
├── Properties
│   ├── width: float
│   ├── height: float
│   ├── rotation: float
│   └── color: vec3
└── Methods
    ├── checkCollision(other: PhysicsObject): bool
    └── draw(): void
```

## Sequence Diagram Flow
1. Main Loop
2. Physics Update
3. Collision Detection
4. Collision Resolution
5. Rendering

## Activity Diagram Flow
1. Program Initialization
2. Main Game Loop
   - Input Processing
   - Physics Update
   - Collision Detection
   - Rendering
3. Program Termination
