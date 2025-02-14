#pragma once
#include <vector>
#include <memory>
#include "PhysicsObject.hpp"

class PhysicsWorld {
private:
    std::vector<std::shared_ptr<PhysicsObject>> objects;
    glm::vec2 gravity{0.0f, -9.81f};
    float drag{0.01f};
    float windowWidth{2.0f};  // OpenGL coordinates (-1 to 1)
    float windowHeight{2.0f}; // OpenGL coordinates (-1 to 1)

public:
    PhysicsWorld(float width = 2.0f, float height = 2.0f)
        : windowWidth(width)
        , windowHeight(height)
    {}

    void addObject(std::shared_ptr<PhysicsObject> obj) {
        objects.push_back(obj);
    }

    void setGravity(const glm::vec2& g) { gravity = g; }
    void setDrag(float d) { drag = d; }
    
    const std::vector<std::shared_ptr<PhysicsObject>>& getObjects() const { return objects; }
    
    void update(float deltaTime);
    void checkCollisions();
    void draw() const;
    void applyForces(float deltaTime);
    void checkBoundaries();
    
    // Find object at position (for mouse interaction)
    std::shared_ptr<PhysicsObject> findObjectAtPosition(const glm::vec2& pos);
};
