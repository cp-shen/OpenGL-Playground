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
        void setVertexData(const GLfloat* data, unsigned count);
        void setIndexData(const GLuint* data, unsigned count);
        void getVertexData(GLfloat* buf);
        void getIndexData(GLuint* buf);
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

inline Matrix operator*(const Matrix& left, const Matrix& right) {
    return MatrixMultiply(right, left);
}

struct Transform {
    public:
        Vector3 scale;
        Quaternion rotation;
        Vector3 translation;

        Transform() {
            rotation = QuaternionIdentity();
            scale = Vector3One();
            translation = Vector3Zero();
        }

        Matrix toMatrix() {
            Matrix mScale = MatrixScale(scale.x, scale.y, scale.z);
            Matrix mRotate = QuaternionToMatrix(rotation);
            Matrix mTranslate = MatrixTranslate(translation.x, translation.y, translation.z);

            Matrix result = mTranslate * mRotate * mScale;
            return result;
        }
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

struct Camera {
    public:
        float fov; // field of view, in radians
        float aspect; // width / height;
        float near;
        float far;

        Vector3 position;
        Quaternion rotation;

        Matrix projectionMatrix() {
            return MatrixPerspective(fov, aspect, near, far);
        }

        Matrix viewMatrix() {
            Vector3 forwardDir = Vector3Zero();
            forwardDir.z = 1;
            forwardDir = Vector3RotateByQuaternion(forwardDir, rotation);

            Vector3 target = Vector3Add(position, forwardDir);

            Vector3 upDir = Vector3Zero();
            upDir.y = 1;
            forwardDir = Vector3RotateByQuaternion(upDir, rotation);

            return MatrixLookAt(position, target, upDir);
        }

        Camera() {
            fov = 45.0f * DEG2RAD;
            aspect = 4.0f / 3.0f;
            near = 0.1f;
            far = 100.0f;

            position = Vector3Zero();
            rotation = QuaternionIdentity();
        }
};

} // namespace GLPractice

#endif // COMMON_H
