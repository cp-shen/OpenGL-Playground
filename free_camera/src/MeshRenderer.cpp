#include "common.h"
#include <stdexcept>

using namespace GLPractice;

MeshRenderer::MeshRenderer(Mesh* mesh, GLProgram* program):
    _mesh(mesh),
    _shaderProgram(program)
{
    if(!_mesh)
        throw std::runtime_error("mesh is null in MeshRenderer constructor");

    if(!_shaderProgram)
        throw std::runtime_error("shaderProgram is null in MeshRenderer constructor");

    load();
}

MeshRenderer::~MeshRenderer() {
    unload();
}

void MeshRenderer::load() {
    if(!_mesh) {
        throw std::runtime_error("no mesh data in MeshRenderer");
    }
    if(!_shaderProgram) {
        throw std::runtime_error("no shader program in MeshRenderer");
    }

    if(!_mesh->vao() || !_mesh->vbo() || !_mesh->ebo()) {
        _mesh->load();
    }

    glBindVertexArray(_mesh->vao());
    glBindBuffer(GL_ARRAY_BUFFER, _mesh->vbo());

    glEnableVertexAttribArray(_shaderProgram->GetAttribLocation(VERT_SHADER_POS_ATTRIB_NAME));
    glVertexAttribPointer(_shaderProgram->GetAttribLocation(VERT_SHADER_POS_ATTRIB_NAME), 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshRenderer::unload() {

}
