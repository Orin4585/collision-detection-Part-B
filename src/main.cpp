#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>
#include "../include/PhysicsWorld.hpp"
#include "../include/Circle.hpp"
#include "../include/Rectangle.hpp"
#include "../include/Polygon.hpp"

// Global variables
PhysicsWorld physicsWorld;
float lastTime = 0.0f;
bool isDragging = false;
std::shared_ptr<PhysicsObject> draggedObject;
glm::vec2 dragStartPos;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        // Convert screen coordinates to world coordinates
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glm::vec2 worldPos(
            (xpos / width) * 2.0f - 1.0f,
            -((ypos / height) * 2.0f - 1.0f)
        );

        if (action == GLFW_PRESS) {
            isDragging = true;
            dragStartPos = worldPos;
        } else if (action == GLFW_RELEASE && isDragging) {
            isDragging = false;
            if (draggedObject) {
                // Set velocity based on drag distance
                glm::vec2 dragVec = worldPos - dragStartPos;
                draggedObject->setVelocity(dragVec * 5.0f);
                draggedObject = nullptr;
            }
        }
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_C: {
                // Create circle at random position
                float x = (rand() % 100 - 50) / 50.0f;
                float y = (rand() % 100 - 50) / 50.0f;
                auto circle = std::make_shared<Circle>(glm::vec2(x, y), 0.1f, 1.0f);
                physicsWorld.addObject(circle);
                break;
            }
            case GLFW_KEY_P: {
                // Create a pentagon at random position
                float x = (rand() % 100 - 50) / 50.0f;
                float y = (rand() % 100 - 50) / 50.0f;
                
                // Create pentagon vertices
                std::vector<glm::vec2> pentagonVerts;
                const float size = 0.1f;  // Size of the pentagon
                for (int i = 0; i < 5; i++) {
                    float angle = (i * 2.0f * 3.14159f) / 5.0f;
                    pentagonVerts.push_back(size * glm::vec2(cos(angle), sin(angle)));
                }
                
                auto pentagon = std::make_shared<Polygon>(glm::vec2(x, y), pentagonVerts, 1.0f);
                pentagon->setColor(glm::vec3(0.2f, 0.8f, 0.3f));  // Green color
                physicsWorld.addObject(pentagon);
                break;
            }
            case GLFW_KEY_T: {
                // Create a triangle at random position
                float x = (rand() % 100 - 50) / 50.0f;
                float y = (rand() % 100 - 50) / 50.0f;
                
                // Create triangle vertices
                std::vector<glm::vec2> triangleVerts = {
                    glm::vec2(-0.1f, -0.1f),
                    glm::vec2(0.1f, -0.1f),
                    glm::vec2(0.0f, 0.1f)
                };
                
                auto triangle = std::make_shared<Polygon>(glm::vec2(x, y), triangleVerts, 1.0f);
                triangle->setColor(glm::vec3(0.8f, 0.2f, 0.3f));  // Red color
                physicsWorld.addObject(triangle);
                break;
            }
            case GLFW_KEY_R: {
                // Create rectangle at random position
                float x = (rand() % 100 - 50) / 50.0f;
                float y = (rand() % 100 - 50) / 50.0f;
                auto rect = std::make_shared<Rectangle>(glm::vec2(x, y), 0.2f, 0.15f, 1.0f);
                physicsWorld.addObject(rect);
                break;
            }
        }
    }
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 600, "2D Collision Simulation", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    
    // Set callbacks
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    // Create initial objects
    auto circle1 = std::make_shared<Circle>(glm::vec2(-0.5f, 0.0f), 0.1f, 1.0f);
    auto circle2 = std::make_shared<Circle>(glm::vec2(0.5f, 0.0f), 0.1f, 1.0f);
    auto rect1 = std::make_shared<Rectangle>(glm::vec2(0.0f, 0.5f), 0.2f, 0.15f, 1.0f);
    
    circle1->setVelocity(glm::vec2(0.5f, 0.0f));
    circle2->setVelocity(glm::vec2(-0.5f, 0.0f));
    
    physicsWorld.addObject(circle1);
    physicsWorld.addObject(circle2);
    physicsWorld.addObject(rect1);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Update physics
        physicsWorld.update(deltaTime);
        
        // Draw all objects
        physicsWorld.draw();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
