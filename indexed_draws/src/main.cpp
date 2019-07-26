#include "common.h"

#include <stdexcept>
#include <iostream>
#include <string>
#include <GLFW/glfw3.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

using namespace GLPractice;

GLProgram* g_program = NULL;
Mesh* g_mesh = NULL;
MeshRenderer* g_meshRenderer = NULL;

GLFWwindow* g_window = NULL;
std::string g_vShaderPath = "../shaders/vShader.vert";
std::string g_fShaderPath = "../shaders/fShader.frag";

void release(){
    if(!g_program){
        delete g_program;
        g_program = NULL;
    }
    if(!g_mesh){
        delete g_mesh;
        g_mesh = NULL;
    }
    if(!g_meshRenderer) {
        delete g_meshRenderer;
        g_meshRenderer = NULL;
    }
}

void loadShaders() {
    const GLShader& vShader = GLShader::shaderFromFile(g_vShaderPath.c_str(), GL_VERTEX_SHADER);
    const GLShader& fShader = GLShader::shaderFromFile(g_fShaderPath.c_str(), GL_FRAGMENT_SHADER);
    GLuint shaders[2] {vShader.getObjectId(), fShader.getObjectId()};

    g_program = new GLProgram(shaders, 2);
}

void loadMeshData() {
    GLfloat vertexData[] {
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
    };

    GLuint indexData[] {
        // default is counter-clockwise
        // use your right hand to judge
        0, 1, 2,
        0, 3, 2,
        0, 1, 3,
        1, 3, 2,
    };

    g_mesh = new Mesh();
    g_mesh->setVertexData(vertexData, sizeof(vertexData) / sizeof(GLfloat));
    g_mesh->setIndexData(indexData, sizeof(indexData) / sizeof(GLuint));
    g_mesh->load();

    g_meshRenderer = new MeshRenderer(g_mesh, g_program);
}

void render() {
    // clear color and depth info since last draw
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // setup defore drawing
    glUseProgram(g_program->getObjectId());
    glBindVertexArray(g_mesh->vao());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_mesh->ebo());

    // draw some primitives
    //glDrawArrays(GL_TRIANGLES, 0, 4);
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

    // reset after drawing
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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

    // enable depth testing
    glEnable(GL_DEPTH_TEST);

    printGLInfo();
    loadShaders();
    loadMeshData();
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
                << g_fShaderPath << std::endl;
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
