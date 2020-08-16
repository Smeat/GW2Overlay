#include "Shader.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <fstream>
#include <iostream>
#include <sstream>

int compile_shader(const char* src, int type) {
	int shader_id = glCreateShader(type);
	printf("Created shader with id %d\n", shader_id);
	glShaderSource(shader_id, 1, &src, NULL);
	glCompileShader(shader_id);
	int  success;
	char infoLog[512];

	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
	if(!success){
		glGetShaderInfoLog(shader_id, 512, NULL, infoLog);
		std::cerr << "Error during Shader compilation: " << infoLog << std::endl;
		glDeleteShader(shader_id);
		shader_id = 0;
	}
	return shader_id;
}

int link_shader(int vertex_shader, int fragment_shader) {
	int shader_prog = glCreateProgram();
	glAttachShader(shader_prog, vertex_shader);
	glAttachShader(shader_prog, fragment_shader);
	glLinkProgram(shader_prog);

	int success;
	glGetProgramiv(shader_prog, GL_COMPILE_STATUS, &success);
	char infoLog[512];
	if(!success){
		glGetProgramInfoLog(shader_prog, 512, NULL, infoLog);
		std::cerr << "Error during Shader linking: " << infoLog << std::endl;
		glDeleteProgram(shader_prog);
		shader_prog = 0;
	}
	return shader_prog;
}

void Shader::load_from_string(const std::string& vertex_src, const std::string& fragment_src) {
	int v_shader_id = compile_shader(vertex_src.c_str(), GL_VERTEX_SHADER);
	int f_shader_id = compile_shader(fragment_src.c_str(), GL_FRAGMENT_SHADER);
	this->m_program_id = link_shader(v_shader_id, f_shader_id);
	glDeleteShader(v_shader_id);
	glDeleteShader(f_shader_id);
}

Shader::Shader(const std::string& vertex_path, const std::string& fragment_path) {
	std::ifstream v_file(vertex_path);
	std::ifstream f_file(fragment_path);

	if(v_file.is_open() && f_file.is_open()) {
		std::stringstream v_stream, f_stream;

		v_stream << v_stream.rdbuf();
		f_stream << f_stream.rdbuf();
		this->load_from_string(v_stream.str(), f_stream.str());
	}
}

void Shader::set_active() {
	glUseProgram(this->m_program_id);
}

void Shader::set_mat4(const std::string& name, const glm::mat4& mat) {
	glUniformMatrix4fv(glGetUniformLocation(this->m_program_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
