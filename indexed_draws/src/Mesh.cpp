#include "common.h"

#include <stdexcept>

using namespace GLPractice;

Mesh::Mesh()
: _vertexCount(0), _indexCount(0), _vertexData(NULL), _indexData(NULL) {
    _worldPosition = glm::vec3(1.0);
}

Mesh::~Mesh() {
    if(_vertexData)
        delete[] _vertexData;
    if(_indexData)
        delete[] _indexData;
}

void Mesh::setVertexData(const float* data, unsigned count) {
    if(!data || count <= 0)
        throw std::runtime_error("No vertex data");

    if(_vertexData)
        delete[] _vertexData;

    _vertexCount = count;
    _vertexData = new GLfloat[count];
    memcpy(_vertexData, data, count);
}

void Mesh::setIndexData(const unsigned* data, unsigned count) {
    if(!data || count <= 0)
        throw std::runtime_error("No index data");

    if(_indexData)
        delete[] _indexData;

    _indexCount = count;
    _indexData = new GLuint[count];
    memcpy(_indexData, data, count);
}

void Mesh::getVertexData(float* buf) {
    memcpy(buf, _vertexData, _vertexCount);
}

void Mesh::getIndexData(unsigned* buf) {
    memcpy(buf, _vertexData, _vertexCount);
}


unsigned Mesh::getVertexCount() {
    return _vertexCount;
}

unsigned Mesh::getIndexCount() {
    return _indexCount;
}

glm::vec3 Mesh::getWorldPosition() {
    return _worldPosition;
}

void Mesh::setWorldPosition(const glm::vec3& position) {
    _worldPosition = position;
}
