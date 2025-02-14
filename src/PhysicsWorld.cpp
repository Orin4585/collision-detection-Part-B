#include "../include/PhysicsWorld.hpp"
#include "../include/Circle.hpp"
#include "../include/Rectangle.hpp"
#include "../include/Polygon.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>

void PhysicsWorld::update(float deltaTime) {
    applyForces(deltaTime);
    checkCollisions();
    checkBoundaries();
    
    // Update positions
    for (auto& obj : objects) {
        obj->update(deltaTime);
    }
}

void PhysicsWorld::applyForces(float deltaTime) {
    for (auto& obj : objects) {
        if (!obj->getIsStatic()) {
            // Apply gravity
            obj->setAcceleration(gravity);
            
            // Apply drag force (reduced drag)
            glm::vec2 dragForce = -drag * 0.5f * obj->getVelocity();
            obj->setAcceleration(obj->getAcceleration() + dragForce / obj->getMass());
        }
    }
}

void PhysicsWorld::checkCollisions() {
    for (size_t i = 0; i < objects.size(); i++) {
        for (size_t j = i + 1; j < objects.size(); j++) {
            auto& obj1 = objects[i];
            auto& obj2 = objects[j];
            
            // Skip if both objects are static
            if (obj1->getIsStatic() && obj2->getIsStatic()) continue;
            
            bool collision = obj1->checkCollision(*obj2) || obj2->checkCollision(*obj1);
            if (collision) {
                // Handle Circle-Circle collision
                if (auto circle1 = std::dynamic_pointer_cast<Circle>(obj1)) {
                    if (auto circle2 = std::dynamic_pointer_cast<Circle>(obj2)) {
                        circle1->resolveCollision(*circle2);
                    }
                    else if (auto rect2 = std::dynamic_pointer_cast<Rectangle>(obj2)) {
                        // Only let circle handle circle-rectangle collision
                        circle1->resolveCollision(*rect2);
                    }
                    else if (auto poly2 = std::dynamic_pointer_cast<Polygon>(obj2)) {
                        // Let polygon handle circle-polygon collision
                        poly2->resolveCollision(*circle1);
                    }
                }
                // Handle Rectangle-Rectangle collision
                else if (auto rect1 = std::dynamic_pointer_cast<Rectangle>(obj1)) {
                    if (auto rect2 = std::dynamic_pointer_cast<Rectangle>(obj2)) {
                        rect1->resolveCollision(*rect2);
                    }
                    else if (auto circle2 = std::dynamic_pointer_cast<Circle>(obj2)) {
                        // Let circle handle circle-rectangle collision
                        circle2->resolveCollision(*rect1);
                    }
                    else if (auto poly2 = std::dynamic_pointer_cast<Polygon>(obj2)) {
                        // Let polygon handle polygon-rectangle collision
                        poly2->resolveCollision(*rect1);
                    }
                }
                // Handle Polygon collisions
                else if (auto poly1 = std::dynamic_pointer_cast<Polygon>(obj1)) {
                    if (auto poly2 = std::dynamic_pointer_cast<Polygon>(obj2)) {
                        poly1->resolveCollision(*poly2);
                    }
                    else if (auto circle2 = std::dynamic_pointer_cast<Circle>(obj2)) {
                        // Handle polygon-circle collision
                        poly1->resolveCollision(*circle2);
                    }
                    else if (auto rect2 = std::dynamic_pointer_cast<Rectangle>(obj2)) {
                        // Handle polygon-rectangle collision
                        poly1->resolveCollision(*rect2);
                    }
                }
            }
        }
    }
}

void PhysicsWorld::checkBoundaries() {
    const float BOUNCE_FACTOR = 0.8f;
    
    for (auto& obj : objects) {
        if (obj->getIsStatic()) continue;
        
        glm::vec2 pos = obj->getPosition();
        glm::vec2 vel = obj->getVelocity();
        
        // Handle circle boundaries
        if (auto circle = std::dynamic_pointer_cast<Circle>(obj)) {
            float radius = circle->getRadius();
            
            // Left and right boundaries
            if (pos.x - radius < -windowWidth/2) {
                pos.x = -windowWidth/2 + radius;
                vel.x = -vel.x * BOUNCE_FACTOR;
            } else if (pos.x + radius > windowWidth/2) {
                pos.x = windowWidth/2 - radius;
                vel.x = -vel.x * BOUNCE_FACTOR;
            }
            
            // Top and bottom boundaries
            if (pos.y - radius < -windowHeight/2) {
                pos.y = -windowHeight/2 + radius;
                vel.y = -vel.y * BOUNCE_FACTOR;
            } else if (pos.y + radius > windowHeight/2) {
                pos.y = windowHeight/2 - radius;
                vel.y = -vel.y * BOUNCE_FACTOR;
            }
        }
        // Handle rectangle boundaries
        else if (auto rect = std::dynamic_pointer_cast<Rectangle>(obj)) {
            float halfWidth = rect->getWidth() / 2;
            float halfHeight = rect->getHeight() / 2;
            
            // Left and right boundaries
            if (pos.x - halfWidth < -windowWidth/2) {
                pos.x = -windowWidth/2 + halfWidth;
                vel.x = -vel.x * BOUNCE_FACTOR;
            } else if (pos.x + halfWidth > windowWidth/2) {
                pos.x = windowWidth/2 - halfWidth;
                vel.x = -vel.x * BOUNCE_FACTOR;
            }
            
            // Top and bottom boundaries
            if (pos.y - halfHeight < -windowHeight/2) {
                pos.y = -windowHeight/2 + halfHeight;
                vel.y = -vel.y * BOUNCE_FACTOR;
            } else if (pos.y + halfHeight > windowHeight/2) {
                pos.y = windowHeight/2 - halfHeight;
                vel.y = -vel.y * BOUNCE_FACTOR;
            }
        }
        // Handle polygon boundaries
        else if (auto poly = std::dynamic_pointer_cast<Polygon>(obj)) {
            auto vertices = poly->getWorldVertices();
            bool needsAdjustment = false;
            glm::vec2 adjustment(0.0f);
            
            // Check all vertices against boundaries
            for (const auto& vertex : vertices) {
                // Left boundary
                if (vertex.x < -windowWidth/2) {
                    float dx = -windowWidth/2 - vertex.x;
                    adjustment.x = std::max(adjustment.x, dx);
                    needsAdjustment = true;
                }
                // Right boundary
                else if (vertex.x > windowWidth/2) {
                    float dx = windowWidth/2 - vertex.x;
                    adjustment.x = std::min(adjustment.x, dx);
                    needsAdjustment = true;
                }
                
                // Bottom boundary
                if (vertex.y < -windowHeight/2) {
                    float dy = -windowHeight/2 - vertex.y;
                    adjustment.y = std::max(adjustment.y, dy);
                    needsAdjustment = true;
                }
                // Top boundary
                else if (vertex.y > windowHeight/2) {
                    float dy = windowHeight/2 - vertex.y;
                    adjustment.y = std::min(adjustment.y, dy);
                    needsAdjustment = true;
                }
            }
            
            // Apply adjustment and bounce if needed
            if (needsAdjustment) {
                pos += adjustment;
                if (adjustment.x != 0.0f) vel.x = -vel.x * BOUNCE_FACTOR;
                if (adjustment.y != 0.0f) vel.y = -vel.y * BOUNCE_FACTOR;
            }
        }
        
        obj->setPosition(pos);
        obj->setVelocity(vel);
    }
}

void PhysicsWorld::draw() const {
    for (const auto& obj : objects) {
        obj->draw();
    }
}

std::shared_ptr<PhysicsObject> PhysicsWorld::findObjectAtPosition(const glm::vec2& pos) {
    // Check circles first (more precise for clicking)
    for (auto it = objects.rbegin(); it != objects.rend(); ++it) {
        if (auto circle = std::dynamic_pointer_cast<Circle>(*it)) {
            float distance = glm::length(circle->getPosition() - pos);
            if (distance <= circle->getRadius()) {
                return *it;
            }
        }
    }
    
    // Then check rectangles
    for (auto it = objects.rbegin(); it != objects.rend(); ++it) {
        if (auto rect = std::dynamic_pointer_cast<Rectangle>(*it)) {
            glm::vec2 rectPos = rect->getPosition();
            float halfWidth = rect->getWidth() / 2;
            float halfHeight = rect->getHeight() / 2;
            
            if (pos.x >= rectPos.x - halfWidth && pos.x <= rectPos.x + halfWidth &&
                pos.y >= rectPos.y - halfHeight && pos.y <= rectPos.y + halfHeight) {
                return *it;
            }
        }
    }
    
    return nullptr;
}
