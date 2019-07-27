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
Transform g_modelTransform;
Camera g_camera;

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

void printMatrix(Matrix mat) {
    std::cout << "in printMatrix:" << std::endl;

    float16 arr = MatrixToFloatV(mat);

    for(unsigned i = 0; i < 16; i++) {
        if(i % 4 == 0)
            std::cout << std::endl;

        std::cout << arr.v[i];
    }
    std::cout << std::endl;
}

void loadShaders() {
    const GLShader& vShader = GLShader::shaderFromFile(g_vShaderPath.c_str(), GL_VERTEX_SHADER);
    const GLShader& fShader = GLShader::shaderFromFile(g_fShaderPath.c_str(), GL_FRAGMENT_SHADER);
    GLuint shaders[2] {vShader.getObjectId(), fShader.getObjectId()};

    g_program = new GLProgram(shaders, 2);
}

void loadMeshData() {
    // a cube
    GLfloat vertexData[] {
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,

        0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,
    };

    GLuint indexData[] {
        // by default it is counter-clockwise
        // use right hand to judge
        7, 3, 2,
        7, 2, 6,
        6, 2, 5,
        5, 2, 1,
        4, 5, 1,
        4, 1, 0,
        7, 4, 3,
        3, 4, 0,
        4, 7, 6,
        4, 6, 5,
        3, 0, 2,
        2, 0, 1,
    };

    g_mesh = new Mesh();
    g_mesh->setVertexData(vertexData, sizeof(vertexData) / sizeof(GLfloat));
    g_mesh->setIndexData(indexData, sizeof(indexData) / sizeof(GLuint));
    g_mesh->load();

    g_meshRenderer = new MeshRenderer(g_mesh, g_program);
}

void updateUniform() {
    glUseProgram(g_program->getObjectId());

    GLint modelUniformLoc = g_program->GetUniformLocation("model");
    GLint viewUniformLoc = g_program->GetUniformLocation("view");
    GLint projUniformLoc = g_program->GetUniformLocation("projection");

    Vector3 yAxis;
    yAxis.x = 0.0f;
    yAxis.y = 1.0f;
    yAxis.z = 0.0f;
    g_modelTransform.rotation =
        QuaternionMultiply(g_modelTransform.rotation, QuaternionFromAxisAngle(yAxis, 0.05f * DEG2RAD));

    g_modelTransform.translation.z = 5.0f;
    g_modelTransform.translation.y = -3.0f;

    float16 modelMatrix = MatrixToFloatV(g_modelTransform.toMatrix());
    glUniformMatrix4fv(modelUniformLoc, 1, GL_FALSE, modelMatrix.v);

    // get user input to update camera info
    // Todo

    float16 viewMatrix = MatrixToFloatV(g_camera.viewMatrix());
    glUniformMatrix4fv(viewUniformLoc, 1, GL_FALSE, viewMatrix.v);

    float16 projMatrix = MatrixToFloatV(g_camera.projectionMatrix());
    glUniformMatrix4fv(projUniformLoc, 1, GL_FALSE, projMatrix.v);

    glUseProgram(0);
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
    glDrawElements(GL_TRIANGLES, 3 * 12, GL_UNSIGNED_INT, 0);

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

    // enable depth testing
    glEnable(GL_DEPTH_TEST);

    // setup viewport
    int bufWidth, bufHeight;
    glfwGetFramebufferSize(g_window, &bufWidth, &bufHeight);
    glViewport(0, 0, bufWidth, bufHeight);

    // setup camera
    g_camera.aspect = (GLfloat) bufWidth / (GLfloat) bufHeight;
    Vector3 xAxis = Vector3Zero();
    xAxis.x = 1;
    g_camera.rotation = QuaternionFromAxisAngle(xAxis, 30.0f * DEG2RAD);

    printGLInfo();
    loadShaders();
    loadMeshData();
}

void appMain() {
    appInit();

    while(!glfwWindowShouldClose(g_window)) {
        glfwPollEvents();
        render();
        updateUniform();
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
