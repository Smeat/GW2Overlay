#ifndef __OBJECT__H__
#define __OBJECT__H__

#include "Shader.h"
#include <glm/fwd.hpp>

#include <vector>
#include <memory>


class Object {

	public:
		Object(std::shared_ptr<Shader> s, std::vector<float> vertex_data);
		void translate(glm::vec3 pos);
		void scale(glm::vec3 scale);
		void rotate(float deg, glm::vec3 v);
		void update();

	

	private:
		std::shared_ptr<Shader> m_shader;
		std::vector<float> m_vertex_data;
		glm::mat4 m_model;

		// position etc
		glm::vec3 m_pos = glm::vec3(0,0,0);
		glm::vec3 m_scale = glm::vec3(1);
		glm::vec3 m_rotation_vec = glm::vec3(1);
		float m_rotation = 0;
};


#endif // __OBJECT__H__
