#include "../include/Circle.hpp"
#include "../include/Rectangle.hpp"
#include <GLFW/glfw3.h>
#include <cmath>

bool Circle::checkCollision(const PhysicsObject& other) const {
    // Check if other object is a circle
    const Circle* otherCircle = dynamic_cast<const Circle*>(&other);
    if (otherCircle) {
        // Calculate distance between centers
        glm::vec2 diff = position - other.getPosition();
        float distance = glm::length(diff);
        
        // If distance is less than sum of radii, collision occurred
        return distance < (radius + otherCircle->getRadius());
    }
    
    // Check if other object is a rectangle
    const Rectangle* rect = dynamic_cast<const Rectangle*>(&other);
    if (rect) {
        // Transform circle center to rectangle's local space
        glm::vec2 localCircleCenter = position - rect->getPosition();
        float cosA = cos(-rect->getRotation());
        float sinA = sin(-rect->getRotation());
        localCircleCenter = glm::vec2(
            localCircleCenter.x * cosA - localCircleCenter.y * sinA,
            localCircleCenter.x * sinA + localCircleCenter.y * cosA
        );
        
        // Find closest point on rectangle to circle center
        float closestX = std::max(-rect->getWidth()/2.0f, std::min(rect->getWidth()/2.0f, localCircleCenter.x));
        float closestY = std::max(-rect->getHeight()/2.0f, std::min(rect->getHeight()/2.0f, localCircleCenter.y));
        
        // Calculate distance between closest point and circle center
        glm::vec2 closest(closestX, closestY);
        glm::vec2 difference = localCircleCenter - closest;
        float distanceSquared = glm::dot(difference, difference);
        
        return distanceSquared <= (radius * radius);
    }
    
    return false;
}

void Circle::resolveCollision(PhysicsObject& other) {
    // Handle circle-circle collision
    Circle* otherCircle = dynamic_cast<Circle*>(&other);
    if (otherCircle) {
        // Calculate collision normal
        glm::vec2 normal = glm::normalize(position - otherCircle->getPosition());
        
        // Relative velocity
        glm::vec2 relativeVel = velocity - otherCircle->getVelocity();
        
        // Calculate relative velocity along the normal
        float velAlongNormal = glm::dot(relativeVel, normal);
        
        // Don't resolve if objects are moving apart
        if (velAlongNormal > 0) return;
        
        // Calculate restitution
        float e = std::min(restitution, otherCircle->restitution);
        
        // Calculate impulse scalar
        float j = -(1 + e) * velAlongNormal;
        j /= 1/mass + 1/otherCircle->getMass();
        
        // Apply impulse
        glm::vec2 impulse = j * normal;
        if (!isStatic) {
            velocity += impulse / mass;
        }
        if (!otherCircle->getIsStatic()) {
            otherCircle->velocity -= impulse / otherCircle->getMass();
        }
        
        // Separate the circles
        float distance = glm::length(position - otherCircle->getPosition());
        float overlap = radius + otherCircle->getRadius() - distance;
        if (overlap > 0) {
            glm::vec2 separation = normal * overlap * 0.5f;
            if (!isStatic) {
                position += separation;
            }
            if (!otherCircle->getIsStatic()) {
                otherCircle->position -= separation;
            }
        }
    }
    
    // Handle circle-rectangle collision
    Rectangle* rect = dynamic_cast<Rectangle*>(&other);
    if (rect) {
        // Transform circle center to rectangle's local space
        glm::vec2 localCircleCenter = position - rect->getPosition();
        float cosA = cos(-rect->getRotation());
        float sinA = sin(-rect->getRotation());
        localCircleCenter = glm::vec2(
            localCircleCenter.x * cosA - localCircleCenter.y * sinA,
            localCircleCenter.x * sinA + localCircleCenter.y * cosA
        );
        
        // Find closest point on rectangle to circle center
        float closestX = std::max(-rect->getWidth()/2.0f, std::min(rect->getWidth()/2.0f, localCircleCenter.x));
        float closestY = std::max(-rect->getHeight()/2.0f, std::min(rect->getHeight()/2.0f, localCircleCenter.y));
        glm::vec2 closestPoint(closestX, closestY);
        
        // Transform closest point back to world space
        glm::vec2 worldClosestPoint = rect->getPosition() + glm::vec2(
            closestPoint.x * cos(rect->getRotation()) - closestPoint.y * sin(rect->getRotation()),
            closestPoint.x * sin(rect->getRotation()) + closestPoint.y * cos(rect->getRotation())
        );
        
        // Calculate collision normal and distance
        glm::vec2 normal = position - worldClosestPoint;
        float distance = glm::length(normal);
        
        // Avoid division by zero
        if (distance < 0.0001f) {
            normal = glm::vec2(1.0f, 0.0f); // Default direction if objects are exactly overlapping
        } else {
            normal /= distance; // Normalize
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
        
        // Apply impulse
        glm::vec2 impulse = j * normal;
        if (!isStatic) {
            velocity += impulse / mass;
        }
        if (!rect->getIsStatic()) {
            rect->setVelocity(rect->getVelocity() - impulse / rect->getMass());
        }
        
        // Calculate and apply minimum separation
        float overlap = radius - distance;
        if (overlap > 0) {
            float totalMass = mass + rect->getMass();
            float ratio1 = !isStatic ? rect->getMass() / totalMass : 0.0f;
            float ratio2 = !rect->getIsStatic() ? mass / totalMass : 0.0f;
            
            // Apply separation based on mass ratio
            if (!isStatic) {
                position += normal * overlap * ratio1;
            }
            if (!rect->getIsStatic()) {
                rect->setPosition(rect->getPosition() - normal * overlap * ratio2);
            }
        }
    }
}
