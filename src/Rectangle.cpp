#include "../include/Rectangle.hpp"
#include "../include/Circle.hpp"
#include <GLFW/glfw3.h>
#include <cmath>

std::vector<glm::vec2> Rectangle::getVertices() const {
    std::vector<glm::vec2> vertices;
    vertices.reserve(4);
    
    float cosA = cos(rotation);
    float sinA = sin(rotation);
    
    // Calculate half dimensions
    float hw = width / 2.0f;
    float hh = height / 2.0f;
    
    // Calculate rotated vertices
    vertices.push_back(position + glm::vec2(hw * cosA - hh * sinA, hw * sinA + hh * cosA));
    vertices.push_back(position + glm::vec2(-hw * cosA - hh * sinA, -hw * sinA + hh * cosA));
    vertices.push_back(position + glm::vec2(-hw * cosA + hh * sinA, -hw * sinA - hh * cosA));
    vertices.push_back(position + glm::vec2(hw * cosA + hh * sinA, hw * sinA - hh * cosA));
    
    return vertices;
}

bool Rectangle::checkCollision(const PhysicsObject& other) const {
    // Check if other object is a circle
    const Circle* circle = dynamic_cast<const Circle*>(&other);
    if (circle) {
        // Handle circle-rectangle collision
        float circleRadius = circle->getRadius();
        glm::vec2 circleCenter = circle->getPosition();
        
        // Transform circle center to rectangle's local space
        glm::vec2 localCircleCenter = circleCenter - position;
        float cosA = cos(-rotation);
        float sinA = sin(-rotation);
        localCircleCenter = glm::vec2(
            localCircleCenter.x * cosA - localCircleCenter.y * sinA,
            localCircleCenter.x * sinA + localCircleCenter.y * cosA
        );
        
        // Find closest point on rectangle to circle center
        float closestX = std::max(-width/2.0f, std::min(width/2.0f, localCircleCenter.x));
        float closestY = std::max(-height/2.0f, std::min(height/2.0f, localCircleCenter.y));
        
        // Calculate distance between closest point and circle center
        glm::vec2 closest(closestX, closestY);
        glm::vec2 difference = localCircleCenter - closest;
        float distanceSquared = glm::dot(difference, difference);
        
        return distanceSquared <= (circleRadius * circleRadius);
    }
    
    // Check if other object is a rectangle
    const Rectangle* rect = dynamic_cast<const Rectangle*>(&other);
    if (rect) {
        // Get vertices of both rectangles
        auto vertices1 = getVertices();
        auto vertices2 = rect->getVertices();
        
        // Separating Axis Theorem (SAT)
        std::vector<glm::vec2> axes;
        
        // Get axes from this rectangle
        for (int i = 0; i < 4; i++) {
            glm::vec2 edge = vertices1[(i + 1) % 4] - vertices1[i];
            axes.push_back(glm::normalize(glm::vec2(-edge.y, edge.x)));
        }
        
        // Get axes from other rectangle
        for (int i = 0; i < 4; i++) {
            glm::vec2 edge = vertices2[(i + 1) % 4] - vertices2[i];
            axes.push_back(glm::normalize(glm::vec2(-edge.y, edge.x)));
        }
        
        // Project vertices onto each axis
        for (const auto& axis : axes) {
            float min1 = INFINITY, max1 = -INFINITY;
            float min2 = INFINITY, max2 = -INFINITY;
            
            // Project first rectangle
            for (const auto& v : vertices1) {
                float proj = glm::dot(v, axis);
                min1 = std::min(min1, proj);
                max1 = std::max(max1, proj);
            }
            
            // Project second rectangle
            for (const auto& v : vertices2) {
                float proj = glm::dot(v, axis);
                min2 = std::min(min2, proj);
                max2 = std::max(max2, proj);
            }
            
            // Check for separation
            if (min1 > max2 || min2 > max1) {
                return false;
            }
        }
        
        return true;
    }
    
    return false;
}

void Rectangle::resolveCollision(PhysicsObject& other) {
    // Handle collision with circle
    Circle* circle = dynamic_cast<Circle*>(&other);
    if (circle) {
        // Transform circle center to rectangle's local space
        glm::vec2 localCircleCenter = circle->getPosition() - position;
        float cosA = cos(-rotation);
        float sinA = sin(-rotation);
        localCircleCenter = glm::vec2(
            localCircleCenter.x * cosA - localCircleCenter.y * sinA,
            localCircleCenter.x * sinA + localCircleCenter.y * cosA
        );
        
        // Find closest point on rectangle to circle center
        float closestX = std::max(-width/2.0f, std::min(width/2.0f, localCircleCenter.x));
        float closestY = std::max(-height/2.0f, std::min(height/2.0f, localCircleCenter.y));
        glm::vec2 closestPoint(closestX, closestY);
        
        // Transform closest point back to world space
        glm::vec2 worldClosestPoint = position + glm::vec2(
            closestPoint.x * cosA - closestPoint.y * sinA,
            closestPoint.x * sinA + closestPoint.y * cosA
        );
        
        // Calculate collision normal
        glm::vec2 normal = glm::normalize(circle->getPosition() - worldClosestPoint);
        
        // Calculate relative velocity
        glm::vec2 relativeVel = velocity - circle->getVelocity();
        
        // Calculate relative velocity along the normal
        float velAlongNormal = glm::dot(relativeVel, normal);
        
        // Don't resolve if objects are moving apart
        if (velAlongNormal > 0) return;
        
        // Calculate restitution
        float e = std::min(restitution, circle->getRestitution());
        
        // Calculate impulse scalar
        float j = -(1 + e) * velAlongNormal;
        j /= 1/mass + 1/circle->getMass();
        
        // Apply impulse
        glm::vec2 impulse = j * normal;
        if (!isStatic) {
            velocity -= impulse / mass;
        }
        if (!circle->getIsStatic()) {
            circle->setVelocity(circle->getVelocity() + impulse / circle->getMass());
        }
        
        // Prevent objects from sticking together by adding a small separation
        float penetrationDepth = circle->getRadius() - glm::length(circle->getPosition() - worldClosestPoint);
        if (penetrationDepth > 0) {
            glm::vec2 separation = normal * (penetrationDepth + 0.001f);
            if (!isStatic) {
                position -= separation * 0.5f;
            }
            if (!circle->getIsStatic()) {
                circle->setPosition(circle->getPosition() + separation * 0.5f);
            }
        }
    }
    
    // Handle collision with rectangle
    Rectangle* rect = dynamic_cast<Rectangle*>(&other);
    if (rect) {
        // Get vertices of both rectangles
        auto vertices1 = getVertices();
        auto vertices2 = rect->getVertices();
        
        // Find the collision normal and minimum overlap using SAT
        glm::vec2 normal(0.0f);
        float minOverlap = INFINITY;
        
        // Check all axes
        std::vector<glm::vec2> axes;
        
        // Get axes from this rectangle
        for (int i = 0; i < 4; i++) {
            glm::vec2 edge = vertices1[(i + 1) % 4] - vertices1[i];
            axes.push_back(glm::normalize(glm::vec2(-edge.y, edge.x)));
        }
        
        // Get axes from other rectangle
        for (int i = 0; i < 4; i++) {
            glm::vec2 edge = vertices2[(i + 1) % 4] - vertices2[i];
            axes.push_back(glm::normalize(glm::vec2(-edge.y, edge.x)));
        }
        
        // Find axis with minimum overlap
        for (const auto& axis : axes) {
            float min1 = INFINITY, max1 = -INFINITY;
            float min2 = INFINITY, max2 = -INFINITY;
            
            // Project first rectangle
            for (const auto& v : vertices1) {
                float proj = glm::dot(v, axis);
                min1 = std::min(min1, proj);
                max1 = std::max(max1, proj);
            }
            
            // Project second rectangle
            for (const auto& v : vertices2) {
                float proj = glm::dot(v, axis);
                min2 = std::min(min2, proj);
                max2 = std::max(max2, proj);
            }
            
            // Calculate overlap
            float overlap = std::min(max1 - min2, max2 - min1);
            
            // If this is the smallest overlap we've found, save it
            if (overlap < minOverlap) {
                minOverlap = overlap;
                normal = axis;
                
                // Make sure normal points from rect1 to rect2
                glm::vec2 center1 = position;
                glm::vec2 center2 = rect->getPosition();
                if (glm::dot(center2 - center1, normal) < 0) {
                    normal = -normal;
                }
            }
        }
        
        // Calculate relative velocity
        glm::vec2 relativeVel = velocity - rect->getVelocity();
        
        // Calculate relative velocity along the normal
        float velAlongNormal = glm::dot(relativeVel, normal);
        
        // Don't resolve if objects are moving apart
        if (velAlongNormal > 0) return;
        
        // Calculate restitution
        float e = std::min(restitution, rect->getRestitution());
        
        // Calculate impulse scalar
        float j = -(1 + e) * velAlongNormal;
        j /= 1/mass + 1/rect->getMass();
        
        // Calculate final impulse with adjusted magnitude
        float dotProduct = std::abs(glm::dot(normal, glm::vec2(1.0f, 0.0f))); // Check if collision is more horizontal
        float impulseFactor = dotProduct > 0.8f ? 0.8f : 1.2f; // Reduce impulse for side collisions
        
        glm::vec2 impulse = j * normal * impulseFactor;
        
        // Apply impulse
        if (!isStatic) {
            velocity -= impulse / mass;
        }
        if (!rect->getIsStatic()) {
            rect->setVelocity(rect->getVelocity() + impulse / rect->getMass());
        }
        
        // Separate the rectangles
        if (minOverlap > 0) {
            float totalMass = mass + rect->getMass();
            float ratio1 = !isStatic ? rect->getMass() / totalMass : 0.0f;
            float ratio2 = !rect->getIsStatic() ? mass / totalMass : 0.0f;
            
            // Add a small extra separation to prevent sticking
            float separationDistance = minOverlap + 0.001f;
            
            // Apply separation based on mass ratio
            if (!isStatic) {
                position -= normal * separationDistance * ratio1;
            }
            if (!rect->getIsStatic()) {
                rect->setPosition(rect->getPosition() + normal * separationDistance * ratio2);
            }
        }
    }
}
