# ifndef COMMON_H
# define COMMON_H

#include <GL/glew.h>
#include <string.h>
#include "../../thirdparty/raymath.h"

#define VERT_SHADER_POS_ATTRIB_NAME "pos"

namespace GLPractice {

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

class GLProgram {
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

class Mesh {
    public:
        Mesh();
        ~Mesh();
        void setVertexData(const float* data, unsigned count);
        void setIndexData(const unsigned* data, unsigned count);
        void getVertexData(float* buf);
        void getIndexData(unsigned* buf);
        unsigned getVertexCount();
        unsigned getIndexCount();
        GLuint vao();
        GLuint vbo();
        GLuint ebo();
        void load();
        void unload();

    private:
        unsigned _vertexCount;
        unsigned _indexCount;
        GLfloat* _vertexData;
        GLuint* _indexData;
        GLuint _vao;
        GLuint _vbo;
        GLuint _ebo;

        // disable copying
        Mesh& operator=(const Mesh& other);
        Mesh(const Mesh& other);
};

struct Transform {
    Vector3 scale;
    Quaternion rotation;
    Vector3 translation;
};

class MeshRenderer {
    public:
        MeshRenderer(Mesh*, GLProgram*);
        ~MeshRenderer();
        void load();
        void unload();
    private:
        Mesh* _mesh;
        GLProgram* _shaderProgram;

        // disable copying
        MeshRenderer(const MeshRenderer& other);
        MeshRenderer& operator=(const MeshRenderer& other);
};

} // namespace GLPractice

#endif // COMMON_H
