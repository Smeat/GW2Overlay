#include "GLRenderer.h"

#include <GL/glew.h>

#include <GL/gl.h>

#include <cstdio>
#include <memory>
#include <vector>

#include "../gl/GLMesh.h"
#include "../gl/GLTexture.h"

void GLRenderer::init() {
	glewInit();
	printf("OpenGL version %s\n", glGetString(GL_VERSION));
	glEnable(GL_DEPTH_TEST);
	glAlphaFunc(GL_GREATER, 0.5f);
	glEnable(GL_ALPHA_TEST);
}

void GLRenderer::update(std::vector<std::shared_ptr<Object>> objects) {
	this->clear();
	for (auto iter = objects.begin(); iter != objects.end(); ++iter) {
		(*iter)->update();
	}
	glFlush();
}
void GLRenderer::clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

std::shared_ptr<Texture> GLRenderer::load_texture(const std::string& path) {
	return std::shared_ptr<Texture>(new GLTexture(path));
}
std::shared_ptr<Mesh> GLRenderer::load_mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
	return std::shared_ptr<Mesh>(new GLMesh(vertices, indices));
}
