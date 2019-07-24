cmake_minimum_required(VERSION 3.13)
project(OpenGL_Pracice)

set(CMAKE_CXX_STANDARD 14)

include_directories(./src)

add_executable(OpenGL_Pracice
    src/main.cpp
    src/GLShader.cpp
    src/GLProgram.cpp
    )

target_link_libraries(OpenGL_Pracice
    GL
    GLEW
    glfw
    )
