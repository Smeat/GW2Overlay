#ifndef __SHADER__H_
#define __SHADER__H_

#include <string>

#include <glm/glm.hpp>

class Shader {
	public:
		Shader() = default;
		Shader(const std::string& vertex_path, const std::string& fragment_path);
		void load_from_string(const std::string& vertex_src, const std::string& fragment_src);
		void set_active();
		void set_mat4(const std::string& name, const glm::mat4& mat);

	private:
		int m_program_id = 0;
};


#endif  // __SHADER__H_
