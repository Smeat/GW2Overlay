#include "Mesh.h"

#include "Texture.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include <GL/glew.h>
#include <GL/gl.h>


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::shared_ptr<Texture> tex) {
	this->m_texture = tex;
	this->m_vertices = vertices;
	this->m_indices = indices;
	glGenVertexArrays(1, &this->m_vao);
	glGenBuffers(1, &this->m_vbo);
	glGenBuffers(1, &this->m_ebo);
	glBindVertexArray(this->m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo);
	glBufferData(GL_ARRAY_BUFFER, this->m_vertices.size() * sizeof(Vertex), &this->m_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(this->m_indices.size()) * sizeof(unsigned int), &this->m_indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));

	glBindVertexArray(0);
}

void Mesh::draw() {
	this->m_texture->set_active();
	glBindVertexArray(this->m_vao);
	glDrawElements(GL_TRIANGLES, this->m_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
