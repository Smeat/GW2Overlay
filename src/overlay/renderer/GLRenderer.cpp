#include "GLRenderer.h"

#include <GL/glew.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <SDL2/SDL_surface.h>

#include <cstdio>
#include <memory>
#include <vector>

#include "gl/GLMesh.h"
#include "gl/GLShader.h"
#include "gl/GLTexture.h"

void GLRenderer::init() {
	glewInit();
	printf("OpenGL version %s\n", glGetString(GL_VERSION));
	glEnable(GL_DEPTH_TEST);
	glAlphaFunc(GL_GREATER, 0.5f);
	glEnable(GL_ALPHA_TEST);
}

void GLRenderer::update() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (auto iter = this->m_objects.begin(); iter != this->m_objects.end(); ++iter) {
		(*iter)->update();
	}
	glFlush();
	glXSwapBuffers(this->m_window.display, this->m_window.window);
}
void GLRenderer::clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glXSwapBuffers(this->m_window.display, this->m_window.window);
}

std::shared_ptr<Texture> GLRenderer::load_texture(SDL_Surface* surf) {
	return std::shared_ptr<Texture>(new GLTexture(surf));
}

std::shared_ptr<Mesh> GLRenderer::load_mesh(VertexList vertices, IndexList indices) {
	return std::shared_ptr<Mesh>(new GLMesh(vertices, indices));
}

std::shared_ptr<Shader> GLRenderer::load_shader(const std::string& vert, const std::string& frag) {
	return std::shared_ptr<Shader>(new GLShader(vert, frag));
}

void GLRenderer::set_objects(std::vector<Object*> objs) { this->m_objects = objs; }
