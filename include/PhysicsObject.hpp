#pragma once
#include <glm/glm.hpp>

class PhysicsObject {
protected:
    glm::vec2 position;    // Position in 2D space
    glm::vec2 velocity;    // Velocity vector
    glm::vec2 acceleration;// Acceleration vector
    float angularVelocity; // Angular velocity (radians per second)
    float rotation;        // Current rotation in radians
    float mass;           // Mass of the object
    float restitution;    // Coefficient of restitution (bounciness)
    float friction;       // Coefficient of friction
    float dragCoefficient;// Air resistance coefficient
    bool isStatic;        // If true, object won't move (like walls)
    static bool showVelocityVectors; // Flag to show/hide velocity vectors
    static const glm::vec2 GRAVITY;  // Gravity vector

public:
    PhysicsObject(const glm::vec2& pos = glm::vec2(0.0f), 
                 float m = 1.0f, 
                 float rest = 0.8f,
                 bool staticObj = false)
        : position(pos)
        , velocity(glm::vec2(0.0f))
        , acceleration(glm::vec2(0.0f))
        , angularVelocity(0.0f)
        , rotation(0.0f)
        , mass(m)
        , restitution(rest)
        , friction(0.3f)
        , dragCoefficient(0.1f)
        , isStatic(staticObj)
    {}

    virtual ~PhysicsObject() = default;

    // Static methods for visualization control
    static void setShowVelocityVectors(bool show) { showVelocityVectors = show; }
    static bool getShowVelocityVectors() { return showVelocityVectors; }

    // Pure virtual functions
    virtual bool checkCollision(const PhysicsObject& other) const = 0;
    virtual void draw() const = 0;

    // Getters
    const glm::vec2& getPosition() const { return position; }
    const glm::vec2& getVelocity() const { return velocity; }
    const glm::vec2& getAcceleration() const { return acceleration; }
    float getAngularVelocity() const { return angularVelocity; }
    float getRotation() const { return rotation; }
    float getMass() const { return mass; }
    float getRestitution() const { return restitution; }
    float getFriction() const { return friction; }
    float getDragCoefficient() const { return dragCoefficient; }
    bool getIsStatic() const { return isStatic; }

    // Setters
    void setPosition(const glm::vec2& pos) { position = pos; }
    void setVelocity(const glm::vec2& vel) { velocity = vel; }
    void setAcceleration(const glm::vec2& acc) { acceleration = acc; }
    void setAngularVelocity(float angVel) { angularVelocity = angVel; }
    void setRotation(float rot) { rotation = rot; }
    void setMass(float m) { mass = m; }
    void setRestitution(float r) { restitution = r; }
    void setFriction(float f) { friction = f; }
    void setDragCoefficient(float d) { dragCoefficient = d; }
    void setStatic(bool s) { isStatic = s; }

    // Apply force
    void applyForce(const glm::vec2& force) {
        if (!isStatic) {
            acceleration += force / mass;
        }
    }

    // Apply impulse
    void applyImpulse(const glm::vec2& impulse) {
        if (!isStatic) {
            velocity += impulse / mass;
        }
    }

    // Apply torque
    void applyTorque(float torque) {
        if (!isStatic) {
            angularVelocity += torque / mass;
        }
    }

    // Physics update
    virtual void update(float deltaTime) {
        if (isStatic) return;

        // Apply gravity
        acceleration += GRAVITY;

        // Apply drag force
        glm::vec2 dragForce = -dragCoefficient * velocity * glm::length(velocity);
        applyForce(dragForce);

        // Update velocity using acceleration
        velocity += acceleration * deltaTime;
        
        // Update position using velocity
        position += velocity * deltaTime;

        // Update rotation using angular velocity
        rotation += angularVelocity * deltaTime;

        // Reset acceleration (forces are accumulated each frame)
        acceleration = glm::vec2(0.0f);
    }
};
