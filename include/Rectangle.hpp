#pragma once
#include "PhysicsObject.hpp"
#include "Renderer.hpp"
#include <vector>

class Rectangle : public PhysicsObject {
private:
    float width;
    float height;
    float rotation; // in radians
    glm::vec3 color;

public:
    Rectangle(const glm::vec2& pos, float w, float h, float m = 1.0f)
        : PhysicsObject(pos, m)
        , width(w)
        , height(h)
        , rotation(0.0f)
        , color(1.0f, 1.0f, 1.0f)  // Default white color
    {}

    float getWidth() const { return width; }
    float getHeight() const { return height; }
    float getRotation() const { return rotation; }
    void setRotation(float r) { rotation = r; }
    void setColor(const glm::vec3& c) { color = c; }
    const glm::vec3& getColor() const { return color; }

    // Get vertices in world space
    std::vector<glm::vec2> getVertices() const;

    bool checkCollision(const PhysicsObject& other) const override;
    void resolveCollision(PhysicsObject& other);
    
    void draw() const override {
        Renderer::drawRectangle(position, width, height, rotation, color);
        if (showVelocityVectors) {
            Renderer::drawVelocityVector(position, velocity * 0.1f);
        }
    }
};
