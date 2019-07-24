# ifndef COMMON_H
# define COMMON_H

#include <GL/glew.h>
#include <string.h>

namespace GLPractice{

class GLShader{
    public:
        GLShader(const char* shaderCode, GLenum shaderType);
        ~GLShader();
        GLuint getObjectId() const;
        static GLShader shaderFromFile(const char* filePath, GLenum shaderType);

    private:
        GLuint _objectId;

        //GLShader(const GLShader& other);
        //GLShader& operator=(const GLShader& other);
};

class GLProgram{
    public:
        GLProgram(const GLuint* shaders, unsigned shaderCount);
        ~GLProgram();
        GLuint getObjectId() const;
        GLint GetAttribLocation(const GLchar* attribName) const;
        GLint GetUniformLocation(const GLchar* uniformName) const;

    private:
        GLuint _objectId;

        //GLProgram(const GLProgram& other);
        //GLProgram& operator=(const GLProgram& other);
};

}
#endif // COMMON_H
