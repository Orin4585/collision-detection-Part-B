#pragma once
#include "PhysicsObject.hpp"
#include "Renderer.hpp"
#include <vector>

class Polygon : public PhysicsObject {
private:
    std::vector<glm::vec2> vertices;  // Local space vertices
    glm::vec3 color;

    // Helper method for polygon-polygon collision detection using SAT
    bool checkPolygonPolygonCollision(const std::vector<glm::vec2>& vertsA, 
                                    const std::vector<glm::vec2>& vertsB) const;

public:
    Polygon(const glm::vec2& pos, const std::vector<glm::vec2>& verts, float m = 1.0f)
        : PhysicsObject(pos, m)
        , vertices(verts)
        , color(1.0f, 1.0f, 1.0f)  // Default white color
    {}

    void setColor(const glm::vec3& c) { color = c; }
    const glm::vec3& getColor() const { return color; }
    const std::vector<glm::vec2>& getLocalVertices() const { return vertices; }

    // Get vertices in world space (transformed by position and rotation)
    std::vector<glm::vec2> getWorldVertices() const;

    bool checkCollision(const PhysicsObject& other) const override;
    void resolveCollision(PhysicsObject& other);
    
    void draw() const override {
        Renderer::drawPolygon(position, vertices, rotation, color);
        if (showVelocityVectors) {
            Renderer::drawVelocityVector(position, velocity * 0.1f);
        }
    }
};
