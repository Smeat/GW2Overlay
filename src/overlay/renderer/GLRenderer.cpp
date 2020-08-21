#include "GLRenderer.h"

#include <GL/glew.h>

#include <GL/gl.h>

#include <cstdio>
#include <memory>
#include <vector>

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
