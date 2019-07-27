#include "common.h"

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>
#include <string>
#include <unordered_set>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

using namespace GLPractice;

Transform g_modelTransform;
Camera g_camera;

GLProgram* g_program = NULL;
Mesh* g_mesh = NULL;
MeshRenderer* g_meshRenderer = NULL;
GLFWwindow* g_window = NULL;

std::unordered_set<void(*)()> g_prerenderCallbacks;

std::string g_vShaderPath = "../shaders/vShader.vert";
std::string g_fShaderPath = "../shaders/fShader.frag";

void appRelease(){
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
    if(!g_window) {
        glfwDestroyWindow(g_window);
    }
    glfwTerminate();
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
    GLfloat vertexData[] {
    // a cube
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,

    // a gizmo
        //0.0f, 0.0f, 0.0f,
        //1.0f, 0.0f, 0.0f,
        //0.0f, 1.0f, 0.0f,
        //0.0f, 0.0f, 1.0f,
    };

    // by default it is counter-clockwise
    // use right hand to judge
    GLuint indexData[] {
    // cube index
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
    // gizmo index
        //0, 1,
        //0, 2,
        //0, 3,
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

    // rotate this model around Y axis by frame
    //Vector3 yAxis = Vector3Zero();
    //yAxis.y = 1.0f;
    //g_modelTransform.rotation =
        //QuaternionMultiply(g_modelTransform.rotation, QuaternionFromAxisAngle(yAxis, 0.05f * DEG2RAD));

    // setting value for each Uniform variable
    float16 modelMatrix = MatrixToFloatV(g_modelTransform.toMatrix());
    glUniformMatrix4fv(modelUniformLoc, 1, GL_FALSE, modelMatrix.v);
    float16 viewMatrix = MatrixToFloatV(g_camera.viewMatrix());
    glUniformMatrix4fv(viewUniformLoc, 1, GL_FALSE, viewMatrix.v);
    float16 projMatrix = MatrixToFloatV(g_camera.projectionMatrix());
    glUniformMatrix4fv(projUniformLoc, 1, GL_FALSE, projMatrix.v);

    glUseProgram(0);
}

void render() {
    // using grey color to clear
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    // clear color and depth info since last draw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // setup defore drawing
    glUseProgram(g_program->getObjectId());
    glBindVertexArray(g_mesh->vao());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_mesh->ebo());

    // draw some primitives
    //glDrawArrays(GL_TRIANGLES, 0, 4);
    glDrawElements(GL_TRIANGLES, 3 * 12, GL_UNSIGNED_INT, 0);

    // reset bindings after drawing
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

static Vector3 moveVelocityRaw = Vector3Zero();

// apply movement to the camera transform along the local XYZ axis
void move() {
    const static float moveSpeed = 0.005f;
    Vector3 _move = Vector3Multiply(moveVelocityRaw, moveSpeed);
    _move = Vector3RotateByQuaternion(_move, g_camera.rotation);
    g_camera.position = Vector3Add(g_camera.position, _move);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // check the window passed in
    if(!window || window != g_window)
        return;

    // Use WASDQE keys to move the camera
    if( key != GLFW_KEY_W &&
        key != GLFW_KEY_A &&
        key != GLFW_KEY_S &&
        key != GLFW_KEY_D &&
        key != GLFW_KEY_Q &&
        key != GLFW_KEY_E )
        return;

    if(action == GLFW_PRESS) {
        g_prerenderCallbacks.insert(move);
        moveVelocityRaw = Vector3Zero();
        switch(key) {
            case GLFW_KEY_W:
                moveVelocityRaw.z = 1.0f;
                break;
            case GLFW_KEY_A:
                moveVelocityRaw.x = 1.0f;
                break;
            case GLFW_KEY_S:
                moveVelocityRaw.z = -1.0f;
                break;
            case GLFW_KEY_D:
                moveVelocityRaw.x = -1.0f;
                break;
            case GLFW_KEY_Q:
                moveVelocityRaw.y = -1.0f;
                break;
            case GLFW_KEY_E:
                moveVelocityRaw.y = 1.0f;
                break;
            default:
                break;
        }
    }
    else if (action == GLFW_RELEASE) {
        g_prerenderCallbacks.erase(move);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    //std::cout << "xoffset: " + std::to_string(xoffset) << std::endl;
    //std::cout << "yoffset: " + std::to_string(yoffset) << std::endl;

    // changing camera fov by yoffset
    float minFov = 10.0f * DEG2RAD;
    float maxFov = 60.0f * DEG2RAD;
    float fovChangeSpeed = 0.5f * DEG2RAD;

    // increasing fov by scroll input
    if(yoffset < 0 && g_camera.fov < maxFov) {
        g_camera.fov += fovChangeSpeed;
    }

    // decreasing fov by scroll input
    if(yoffset > 0 && g_camera.fov > minFov) {
        g_camera.fov -= fovChangeSpeed;
    }
}


void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    static double _xpos = 0.0;
    static double _ypos = 0.0;
    static float _rotX = 0.0f;
    static float _rotY = 0.0f;
    const static float _rotSpeed = 0.05f * DEG2RAD;
    const static float _minRotX = -80.0f * DEG2RAD;
    const static float _maxRotX = 80.0f * DEG2RAD;
    static Quaternion _originalRot = g_camera.rotation;

    if(_xpos != 0.0 || _ypos != 0.0) {
        float deltaX = xpos - _xpos;
        float deltaY = ypos - _ypos;

        _rotX += deltaY * _rotSpeed;
        _rotY -= deltaX * _rotSpeed;

        // clamp rot values
        _rotX = Clamp(_rotX, _minRotX, _maxRotX);
        if(_rotY > PI)
            _rotY -= 2.0f * PI;
        else if(_rotY < -PI)
            _rotY += 2.0f * PI;

        Vector3 xAxis = Vector3Zero();
        xAxis.x = 1.0f;
        Vector3 yAxis = Vector3Zero();
        yAxis.y = 1.0f;

        Quaternion qX = QuaternionFromAxisAngle(xAxis, _rotX);
        Quaternion qY = QuaternionFromAxisAngle(yAxis, _rotY);

        g_camera.rotation = QuaternionMultiply(QuaternionMultiply(
                    qY, _originalRot), qX);

        //std::cout << "_rotX: " << std::to_string(_rotX)
            //<< " _rotY: " << std::to_string(_rotY) << std::endl;
    }

    _xpos = xpos;
    _ypos = ypos;
}

void appInit(){
    // init glfw
    glfwSetErrorCallback(onError);
    if(!glfwInit())
        throw std::runtime_error("glfwInit failed");

    // configure glfw window before creating
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // create glfw window
    g_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
            "OpenGL Practice", NULL, NULL);
    if(!g_window)
        throw std::runtime_error("glfwCreateWindow failed");

    // setup OpenGL context
    glfwMakeContextCurrent(g_window);

    // setup glfw mouse and keyboard input
    // lock cursor to window
    glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //glfwSetInputMode(g_window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetKeyCallback(g_window, key_callback);
    glfwSetScrollCallback(g_window, scroll_callback);
    glfwSetCursorPosCallback(g_window, cursor_pos_callback);

    // init glew
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK)
        throw std::runtime_error("glewInit failed");
    if(!GLEW_VERSION_3_3)
        throw std::runtime_error("OpenGL 3.3 API is not avaliable.");

    // enable depth testing
    // default: choose fragment having smaller depth
    glEnable(GL_DEPTH_TEST);
    // enable face culling
    // default: remain front face, which is counter-clockwise
    glEnable(GL_CULL_FACE);

    // setup viewport
    int bufWidth, bufHeight;
    glfwGetFramebufferSize(g_window, &bufWidth, &bufHeight);
    glViewport(0, 0, bufWidth, bufHeight);

    // setup camera transform
    g_camera.aspect = (GLfloat) bufWidth / (GLfloat) bufHeight;
    // rot 30 degree along X axis
    //Vector3 xAxis = Vector3Zero();
    //xAxis.x = 1;
    //g_camera.rotation = QuaternionFromAxisAngle(xAxis, 30.0f * DEG2RAD);

    // setup model transform
    g_modelTransform.translation.z = 5.0f;
    g_modelTransform.translation.x = 0.0f;
    g_modelTransform.translation.y = -2.0f;

    // load data for redering
    loadShaders();
    loadMeshData();

    // show some system info
    printGLInfo();

    // add pre-render callbacks
    g_prerenderCallbacks.insert(updateUniform);
}

void appMain() {
    appInit();

    while(!glfwWindowShouldClose(g_window)) {
        glfwPollEvents();

        for(void(*func)() : g_prerenderCallbacks) {
            if(func)
                func();
        }

        render();

        glfwSwapBuffers(g_window);
    }

    appRelease();
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
        appRelease();

        std::cerr << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
