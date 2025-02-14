#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

int main() {
    // Test GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }
    std::cout << "GLFW initialized successfully!\n";
    
    // Test GLM
    glm::vec2 testVector(1.0f, 2.0f);
    std::cout << "GLM test vector: (" << testVector.x << ", " << testVector.y << ")\n";
    
    glfwTerminate();
    return 0;
}
