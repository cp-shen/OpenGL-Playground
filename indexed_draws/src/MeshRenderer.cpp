#include "common.h"

using namespace GLPractice;

MeshRenderer::MeshRenderer()
: _mesh(NULL){

}

MeshRenderer::~MeshRenderer() {

}

void MeshRenderer::feedMesh(Mesh* mesh) {
    _mesh = mesh;
}
