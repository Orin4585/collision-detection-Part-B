#define GLM_ENABLE_EXPERIMENTAL
#include "Polygon.hpp"
#include "Circle.hpp"
#include "Rectangle.hpp"
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/norm.hpp>

std::vector<glm::vec2> Polygon::getWorldVertices() const {
    std::vector<glm::vec2> worldVertices;
    worldVertices.reserve(vertices.size());
    
    for (const auto& vertex : vertices) {
        // Rotate the vertex
        glm::vec2 rotatedVertex = glm::rotate(vertex, rotation);
        // Translate to world position
        worldVertices.push_back(position + rotatedVertex);
    }
    
    return worldVertices;
}

bool Polygon::checkCollision(const PhysicsObject& other) const {
    // Get this polygon's vertices in world space
    const auto& worldVerts = getWorldVertices();
    
    // Handle Circle collision
    if (auto circle = dynamic_cast<const Circle*>(&other)) {
        glm::vec2 circleCenter = circle->getPosition();
        float circleRadius = circle->getRadius();
        
        // For each edge of the polygon
        for (size_t i = 0; i < worldVerts.size(); i++) {
            const glm::vec2& v1 = worldVerts[i];
            const glm::vec2& v2 = worldVerts[(i + 1) % worldVerts.size()];
            
            // Get vector from v1 to v2
            glm::vec2 edge = v2 - v1;
            // Get vector from v1 to circle center
            glm::vec2 v1ToCircle = circleCenter - v1;
            
            // Project v1ToCircle onto edge
            float edgeLength = glm::length(edge);
            glm::vec2 edgeNorm = edge / edgeLength;
            float projection = glm::dot(v1ToCircle, edgeNorm);
            
            glm::vec2 closestPoint;
            if (projection <= 0) {
                closestPoint = v1;
            } else if (projection >= edgeLength) {
                closestPoint = v2;
            } else {
                closestPoint = v1 + edgeNorm * projection;
            }
            
            // Check if circle intersects with closest point
            float distanceSquared = glm::length2(circleCenter - closestPoint);
            if (distanceSquared <= circleRadius * circleRadius) {
                return true;
            }
        }
        return false;
    }
    // Handle Rectangle collision
    else if (auto rect = dynamic_cast<const Rectangle*>(&other)) {
        // Get rectangle vertices
        std::vector<glm::vec2> rectVerts;
        glm::vec2 rectPos = rect->getPosition();
        float rectWidth = rect->getWidth();
        float rectHeight = rect->getHeight();
        float rectRot = rect->getRotation();
        
        // Calculate rectangle vertices
        glm::vec2 halfSize(rectWidth/2, rectHeight/2);
        glm::vec2 v1 = glm::rotate(glm::vec2(-halfSize.x, -halfSize.y), rectRot);
        glm::vec2 v2 = glm::rotate(glm::vec2(halfSize.x, -halfSize.y), rectRot);
        glm::vec2 v3 = glm::rotate(glm::vec2(halfSize.x, halfSize.y), rectRot);
        glm::vec2 v4 = glm::rotate(glm::vec2(-halfSize.x, halfSize.y), rectRot);
        
        rectVerts.push_back(rectPos + v1);
        rectVerts.push_back(rectPos + v2);
        rectVerts.push_back(rectPos + v3);
        rectVerts.push_back(rectPos + v4);
        
        // Use SAT for polygon-polygon collision
        return checkPolygonPolygonCollision(worldVerts, rectVerts);
    }
    // Handle Polygon collision
    else if (auto poly = dynamic_cast<const Polygon*>(&other)) {
        return checkPolygonPolygonCollision(worldVerts, poly->getWorldVertices());
    }
    
    return false;
}

bool Polygon::checkPolygonPolygonCollision(const std::vector<glm::vec2>& vertsA, 
                                         const std::vector<glm::vec2>& vertsB) const {
    // Get edges for both polygons
    auto getEdges = [](const std::vector<glm::vec2>& verts) {
        std::vector<glm::vec2> edges;
        for (size_t i = 0; i < verts.size(); i++) {
            glm::vec2 edge = verts[(i + 1) % verts.size()] - verts[i];
            edges.push_back(glm::normalize(edge));
        }
        return edges;
    };
    
    auto edgesA = getEdges(vertsA);
    auto edgesB = getEdges(vertsB);
    
    // Combine all edges to test
    std::vector<glm::vec2> axes;
    for (const auto& edge : edgesA) axes.push_back(glm::vec2(-edge.y, edge.x));
    for (const auto& edge : edgesB) axes.push_back(glm::vec2(-edge.y, edge.x));
    
    // Test projection onto each axis
    for (const auto& axis : axes) {
        float minA = std::numeric_limits<float>::max();
        float maxA = std::numeric_limits<float>::lowest();
        float minB = std::numeric_limits<float>::max();
        float maxB = std::numeric_limits<float>::lowest();
        
        // Project vertices of polygon A
        for (const auto& v : vertsA) {
            float proj = glm::dot(v, axis);
            minA = std::min(minA, proj);
            maxA = std::max(maxA, proj);
        }
        
        // Project vertices of polygon B
        for (const auto& v : vertsB) {
            float proj = glm::dot(v, axis);
            minB = std::min(minB, proj);
            maxB = std::max(maxB, proj);
        }
        
        // Check for separation
        if (maxA < minB || maxB < minA) {
            return false;
        }
    }
    
    return true;
}

void Polygon::resolveCollision(PhysicsObject& other) {
    if (getIsStatic() && other.getIsStatic()) return;

    // Calculate collision normal
    glm::vec2 normal;
    
    if (auto circle = dynamic_cast<Circle*>(&other)) {
        // For circle collision, use direction from polygon center to circle center
        normal = glm::normalize(circle->getPosition() - position);
    }
    else {
        // For polygon collision, use direction between centers
        normal = glm::normalize(other.getPosition() - position);
    }
    
    glm::vec2 relativeVelocity = other.getVelocity() - getVelocity();
    float velocityAlongNormal = glm::dot(relativeVelocity, normal);
    
    // Only resolve if objects are moving toward each other
    if (velocityAlongNormal > 0) return;
    
    float restitutionCombined = std::min(getRestitution(), other.getRestitution());
    float j = -(1.0f + restitutionCombined) * velocityAlongNormal;
    j /= (1.0f / getMass()) + (1.0f / other.getMass());
    
    glm::vec2 impulse = j * normal;
    
    // Apply impulse
    if (!getIsStatic()) setVelocity(getVelocity() - impulse / getMass());
    if (!other.getIsStatic()) other.setVelocity(other.getVelocity() + impulse / other.getMass());
    
    // Prevent overlap by moving objects apart slightly
    const float separationFactor = 0.01f; // Small separation to prevent sticking
    if (!getIsStatic()) setPosition(getPosition() - normal * separationFactor);
    if (!other.getIsStatic()) other.setPosition(other.getPosition() + normal * separationFactor);
}
