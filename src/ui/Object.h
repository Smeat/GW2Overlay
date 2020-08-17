#ifndef __OBJECT__H__
#define __OBJECT__H__

#include "Shader.h"
#include "Mesh.h"
#include <glm/fwd.hpp>

#include <glm/trigonometric.hpp>
#include <vector>
#include <memory>


class Object {

	public:
		Object(std::shared_ptr<Shader> s, std::vector<Mesh> vertex_data);
		void translate(glm::vec3 pos);
		void scale(glm::vec3 scale);
		void rotate(float deg, glm::vec3 v);
		void update();

	

	private:
		std::shared_ptr<Shader> m_shader;
		std::vector<Mesh> m_meshes;
		glm::mat4 m_model;

		// position etc
		glm::vec3 m_pos = glm::vec3(0,0,0);
		glm::vec3 m_scale = glm::vec3(1);
		glm::vec3 m_rotation_vec = glm::vec3(0.0f, 0.0f, 1.0f);
		float m_rotation = glm::radians(180.0f);
};


#endif // __OBJECT__H__
