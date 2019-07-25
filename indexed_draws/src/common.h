# ifndef COMMON_H
# define COMMON_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string.h>

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
        glm::vec3 getWorldPosition();
        void setWorldPosition(const glm::vec3& position);
    private:
        unsigned _vertexCount;
        unsigned _indexCount;
        GLfloat* _vertexData;
        GLuint* _indexData;
        glm::vec3 _worldPosition;

        // disable copying
        Mesh& operator=(const Mesh& other);
        Mesh(const Mesh& other);
};

class MeshRenderer {
    public:
        MeshRenderer();
        ~MeshRenderer();
        MeshRenderer(const MeshRenderer& other);
        MeshRenderer& operator=(const MeshRenderer& other);
        void feedMesh(Mesh* mesh);
    private:
        Mesh* _mesh;
};

} // namespace GLPractice

#endif // COMMON_H
