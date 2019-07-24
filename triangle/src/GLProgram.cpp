#include "common.h"
#include <stdexcept>

using namespace GLPractice;

GLProgram::GLProgram(const GLuint* shaders, unsigned shaderCount){
    if(!shaders || shaderCount <= 0)
        throw std::runtime_error("no shader provided to create the program");

    _objectId = glCreateProgram();
    if(_objectId == 0)
        throw std::runtime_error("glCreateProgram failed");

    for(unsigned i = 0; i < shaderCount; i++){
        if(shaders[i])
            glAttachShader(_objectId, shaders[i]);
    }

    glLinkProgram(_objectId);
    glValidateProgram(_objectId);

    for(unsigned i = 0; i < shaderCount; i++){
        if(shaders[i])
            glDetachShader(_objectId, shaders[i]);
    }

    // check link error
    GLint status;
    glGetProgramiv(_objectId, GL_LINK_STATUS, &status);
    if(status == GL_FALSE){
        std::string msg = "failed to link program: " + std::to_string(_objectId);

        GLint infoLogLength;
        glGetProgramiv(_objectId, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar infoLog[infoLogLength + 1];
        glGetProgramInfoLog(_objectId, infoLogLength, NULL, infoLog);

        glDeleteProgram(_objectId);

        msg += "\n";
        msg += infoLog;
        throw std::runtime_error(msg);
    }
    // check validate error
    glGetProgramiv(_objectId, GL_VALIDATE_STATUS, &status);
    if(status == GL_FALSE){
        std::string msg = "failed to validate program: " + std::to_string(_objectId);

        GLint infoLogLength;
        glGetProgramiv(_objectId, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar infoLog[infoLogLength + 1];
        glGetProgramInfoLog(_objectId, infoLogLength, NULL, infoLog);

        glDeleteProgram(_objectId);

        msg += "\n";
        msg += infoLog;
        throw std::runtime_error(msg);
    }
}

GLProgram::~GLProgram(){
    if(_objectId != 0)
        glDeleteProgram(_objectId);
}

GLuint GLProgram::getObjectId() const {
    return _objectId;
}

GLint GLProgram::GetAttribLocation(const GLchar* attribName) const {
    if(!attribName)
        throw std::runtime_error("attribName is null");

    GLint location = glGetAttribLocation(_objectId, attribName);
    return location;
}

GLint GLProgram::GetUniformLocation(const GLchar* uniformName) const {
    if(!uniformName)
        throw std::runtime_error("uniformName is null");

    GLint location = glGetAttribLocation(_objectId, uniformName);
    return location;
}
