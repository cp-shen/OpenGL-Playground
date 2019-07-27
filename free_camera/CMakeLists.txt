cmake_minimum_required(VERSION 3.13)
project(OpenGL_Practice)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)

include_directories(./src)

add_executable(OpenGL_Pracice
    src/main.cpp
    src/Mesh.cpp
    src/GLShader.cpp
    src/GLProgram.cpp
    src/MeshRenderer.cpp
    )

target_link_libraries(OpenGL_Pracice
    GL
    GLEW
    glfw
    )
