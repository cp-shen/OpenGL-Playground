#include "common.h"

#include <stdexcept>
#include <iostream>
#include <string>
#include <GLFW/glfw3.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define VERT_SHADER_POS_ATTRIB_NAME "pos"

using namespace GLPractice;

GLProgram* g_program = NULL;
GLuint g_vao = 0;
GLFWwindow* g_window = NULL;
std::string g_vShaderPath = "../shaders/vShader.vert";
std::string g_fShaderPath = "../shaders/fShader.frag";

void release(){
    if(!g_program){
        delete g_program;
        g_program = NULL;
    }
}

void loadShaders() {
    const GLShader& vShader = GLShader::shaderFromFile(g_vShaderPath.c_str(), GL_VERTEX_SHADER);
    const GLShader& fShader = GLShader::shaderFromFile(g_fShaderPath.c_str(), GL_FRAGMENT_SHADER);
    GLuint shaders[2] {vShader.getObjectId(), fShader.getObjectId()};

    g_program = new GLProgram(shaders, 2);
}

void loadTriangle() {
    glGenVertexArrays(1, &g_vao);
    glBindVertexArray(g_vao);

    GLuint tmpVBO = 0;
    glGenBuffers(1, &tmpVBO);
    glBindBuffer(GL_ARRAY_BUFFER, tmpVBO);

    GLfloat vertexData[] {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(g_program->GetAttribLocation(VERT_SHADER_POS_ATTRIB_NAME));
    glVertexAttribPointer(g_program->GetAttribLocation(VERT_SHADER_POS_ATTRIB_NAME), 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void render() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(g_program->getObjectId());
    glBindVertexArray(g_vao);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
    glUseProgram(0);
}

void onError(int errorCode, const char* msg) {
    throw std::runtime_error(msg);
}

void printGLInfo() {
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
}

void appInit(){
    // init glfw
    glfwSetErrorCallback(onError);
    if(!glfwInit())
        throw std::runtime_error("glfwInit failed");

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    g_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
            "OpenGL Practice", NULL, NULL);
    if(!g_window)
        throw std::runtime_error("glfwCreateWindow failed");

    glfwMakeContextCurrent(g_window);

    // init glew
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK)
        throw std::runtime_error("glewInit failed");
    if(!GLEW_VERSION_3_3)
        throw std::runtime_error("OpenGL 3.3 API is not avaliable.");

    // setup viewport
    int bufWidth, bufHeight;
    glfwGetFramebufferSize(g_window, &bufWidth, &bufHeight);
    glViewport(0, 0, bufWidth, bufHeight);

    printGLInfo();
    loadShaders();
    loadTriangle();
}

void appMain() {
    appInit();

    while(!glfwWindowShouldClose(g_window)) {
        glfwPollEvents();
        render();
        glfwSwapBuffers(g_window);
    }

    glfwTerminate();
    release();
}

int main(int argc, char* argv[]) {
    try {
        if(argc >= 3) {
            g_vShaderPath = argv[1];
            g_vShaderPath = argv[2];
        }
        else {
            std::cout << "No shader path provided." << std::endl
                << "Use default shader path:" << std::endl
                << g_vShaderPath << std::endl
                << g_fShaderPath << std::endl
                ;
        }

        appMain();
    }
    catch(const std::exception& e) {
        glfwTerminate();
        release();

        std::cerr << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
