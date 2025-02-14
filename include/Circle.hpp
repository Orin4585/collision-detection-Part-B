#pragma once
#include "PhysicsObject.hpp"
#include "Renderer.hpp"

class Circle : public PhysicsObject {
private:
    float radius;
    glm::vec3 color;

public:
    Circle(const glm::vec2& pos, float r, float m = 1.0f)
        : PhysicsObject(pos, m)
        , radius(r)
        , color(1.0f, 1.0f, 1.0f)  // Default white color
    {}

    float getRadius() const { return radius; }
    void setColor(const glm::vec3& c) { color = c; }
    const glm::vec3& getColor() const { return color; }

    bool checkCollision(const PhysicsObject& other) const override;
    void resolveCollision(PhysicsObject& other);
    
    void draw() const override {
        Renderer::drawCircle(position, radius, color);
        if (showVelocityVectors) {
            Renderer::drawVelocityVector(position, velocity * 0.1f);
        }
    }
};
