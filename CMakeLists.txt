cmake_minimum_required(VERSION 3.10)
project(CollisionSimulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Add include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/libs/glfw/include)
include_directories(${CMAKE_SOURCE_DIR}/libs/glm)

# Add source files
set(SOURCES
    src/main.cpp
    src/Circle.cpp
    src/Rectangle.cpp
    src/PhysicsWorld.cpp
    src/PhysicsObject.cpp
    src/Polygon.cpp
)

# Set GLFW paths
set(GLFW_DLL ${CMAKE_SOURCE_DIR}/libs/glfw/lib-mingw-w64/glfw3.dll)
set(GLFW_LIB ${CMAKE_SOURCE_DIR}/libs/glfw/lib-mingw-w64/libglfw3dll.a)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
    OpenGL::GL
    ${GLFW_LIB}
)

# Add compiler definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_DLL)

# Copy GLFW DLL to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${GLFW_DLL}
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

# Add test installation executable
add_executable(test_installation
    src/test_installation.cpp
)

target_link_libraries(test_installation
    PRIVATE 
    OpenGL::GL
    ${GLFW_LIB}
)

# Copy GLFW DLL for test_installation
add_custom_command(TARGET test_installation POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${GLFW_DLL}
    $<TARGET_FILE_DIR:test_installation>
)
