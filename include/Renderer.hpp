#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

class Renderer {
public:
    static void drawCircle(const glm::vec2& center, float radius, const glm::vec3& color = glm::vec3(1.0f)) {
        const int segments = 32;
        glBegin(GL_TRIANGLE_FAN);
        glColor3f(color.x, color.y, color.z);
        glVertex2f(center.x, center.y);
        for (int i = 0; i <= segments; i++) {
            float angle = 2.0f * 3.14159f * float(i) / float(segments);
            glVertex2f(center.x + radius * cosf(angle), 
                      center.y + radius * sinf(angle));
        }
        glEnd();
    }

    static void drawRectangle(const glm::vec2& center, float width, float height, 
                            float rotation, const glm::vec3& color = glm::vec3(1.0f)) {
        glPushMatrix();
        glTranslatef(center.x, center.y, 0.0f);
        glRotatef(rotation * 57.2958f, 0.0f, 0.0f, 1.0f); // Convert radians to degrees

        glBegin(GL_QUADS);
        glColor3f(color.x, color.y, color.z);
        glVertex2f(-width/2, -height/2);
        glVertex2f(width/2, -height/2);
        glVertex2f(width/2, height/2);
        glVertex2f(-width/2, height/2);
        glEnd();

        glPopMatrix();
    }

    static void drawCollisionPoint(const glm::vec2& point) {
        glPointSize(5.0f);
        glBegin(GL_POINTS);
        glColor3f(1.0f, 0.0f, 0.0f);  // Red color for collision points
        glVertex2f(point.x, point.y);
        glEnd();
    }

    static void drawVelocityVector(const glm::vec2& start, const glm::vec2& velocity, 
                                 const glm::vec3& color = glm::vec3(0.0f, 1.0f, 0.0f)) {
        glm::vec2 end = start + velocity;
        glBegin(GL_LINES);
        glColor3f(color.x, color.y, color.z);
        glVertex2f(start.x, start.y);
        glVertex2f(end.x, end.y);
        glEnd();
    }

    static void drawPolygon(const glm::vec2& center, const std::vector<glm::vec2>& vertices,
                           float rotation, const glm::vec3& color = glm::vec3(1.0f)) {
        glPushMatrix();
        glTranslatef(center.x, center.y, 0.0f);
        glRotatef(rotation * 57.2958f, 0.0f, 0.0f, 1.0f); // Convert radians to degrees

        glBegin(GL_TRIANGLE_FAN);
        glColor3f(color.x, color.y, color.z);
        
        // Start from center
        glVertex2f(0.0f, 0.0f);
        
        // Draw vertices
        for (const auto& vertex : vertices) {
            glVertex2f(vertex.x, vertex.y);
        }
        // Close the polygon by connecting back to the first vertex
        if (!vertices.empty()) {
            glVertex2f(vertices[0].x, vertices[0].y);
        }
        
        glEnd();
        glPopMatrix();
    }
};
