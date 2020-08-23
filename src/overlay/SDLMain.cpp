/*
 * Copyright (c) 2020 smeat.
 *
 * This file is part of GW2Overlay
 * (see https://github.com/Smeat/GW2Overlay).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/*
 * TODO:
 * - Add GUI (buttons, select files etc)
 *    - Probably using qt
 * - Find better way to start python script
 *    - Currently GW2 needs to be started and then start the mumble script via
 * the wine cmd
 *
 */

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <arpa/inet.h>
#include <locale.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <wchar.h>
#include <filesystem>

#include <boost/program_options.hpp>

#include "Mesh.h"
#include "Object.h"
#include "Shader.h"

#include "../utils/CategoryManager.h"
#include "../utils/GW2Api.h"
#include "../utils/GW2Link.h"
#include "../utils/GW2Map.h"
#include "../utils/POI.h"
#include "../utils/json/json.hpp"
#include "../utils/xml/pugixml.hpp"

#include "QtMain.h"
#include "Texture.h"
#include "Window.h"
#include "renderer/GLRenderer.h"
#include "renderer/Renderer.h"
#include "renderer/VKRenderer.h"

using json = nlohmann::json;
namespace po = boost::program_options;

// TODO: remove this global mess and create more files
std::vector<std::shared_ptr<Object>> objects;
std::shared_ptr<Shader> my_shader;
GW2Api api("./cache");
GW2Map map;

void load_objects(int mapid, std::shared_ptr<Renderer> rend) {
	std::vector<Vertex> vertices;
	vertices.push_back(Vertex(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)));
	vertices.push_back(Vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)));
	vertices.push_back(Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)));
	vertices.push_back(Vertex(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)));
	objects.clear();
	std::unordered_map<std::string, std::shared_ptr<Texture>> texture_file_map;

	auto markers = CategoryManager::getInstance().get_categories();
	auto pois = CategoryManager::getInstance().get_pois();

	auto cube_mesh = rend->load_mesh(vertices, {0, 1, 3, 1, 2, 3});

	for (auto iter = pois->begin(); iter != pois->end(); ++iter) {
		if (iter->m_map_id == mapid) {
			auto cat = MarkerCategory::find_children(*markers, iter->m_type);
			if (!cat) {
				std::cout << "Couldn't find " << iter->m_type << std::endl;
				continue;
			}
			if (!cat->m_enabled) continue;
			// let poi icon file overwrite the category icon file
			std::string icon_file = cat->m_icon_file;
			if (!iter->m_icon_file.empty()) {
				icon_file = iter->m_icon_file;
				std::cout << "Using icon from poi " << icon_file << std::endl;
			}
			if (icon_file.empty()) {
				// use the default icon file
				icon_file = "Data/arrow.png";
			}
			if (texture_file_map.find(icon_file) == texture_file_map.end()) {
				// TODO: handle missing texture
				try {
					auto tex = rend->load_texture(icon_file);
					std::cout << "Loading texture with path " << icon_file << std::endl;
					texture_file_map.insert({icon_file, tex});
				} catch (...) {
					continue;
				}
			}
			auto tex_iter = texture_file_map.find(icon_file);
			// TODO: this creates a new mesh, while they are all the same...
			std::shared_ptr<TexturedMesh> my_mesh(new TexturedMesh(cube_mesh, tex_iter->second));
			std::shared_ptr<Object> obj = rend->load_object(my_shader, {my_mesh});
			auto pos = iter->m_pos;
			pos.y += cat->m_height_offset;
			obj->translate(pos);
			obj->scale({cat->m_icon_size * 1.0f, cat->m_icon_size * 1.0f, 1.0f});
			objects.push_back(obj);
		}
	}
}

void update_camera(const LinkedMem* gw2_data) {
	auto ctx = gw2_data->get_context();
	//	TODO: draw POIs on the map, if it is open
	//	This requires to translate the position of all objects to map
	// coordinates
	// get map bounds from api
	if (ctx->get_ui_state(UI_STATE::MAP_OPEN)) {
		glm::vec3 cameraPos;
		cameraPos.x = map.continentToMapX(ctx->mapCenterX);
		cameraPos.z = map.continentToMapY(ctx->mapCenterY);
		// TODO: find correct scale!
		// 500 is pretty close in LA, but differs from map to map
		// TODO: scale objects up, when on map?
		cameraPos.y = 500 * ctx->mapScale;
		auto cameraFront = glm::vec3(0.0f, -1.0f, 0.0f);
		auto cameraUp = glm::vec3(0.0f, 1.0f, 1.0f);

		auto view = glm::lookAtLH(cameraPos, cameraPos + cameraFront, cameraUp);
		my_shader->set_view(view);
		/*
				printf("map (%f, %f) map player (%f %f) player (%f %f) scale %f\n", ctx->mapCenterX, ctx->mapCenterY,
					   ctx->playerX, ctx->playerY, gw2_data->fAvatarPosition[0], gw2_data->fAvatarPosition[2],
		   ctx->mapScale); printf("cam diff to player (%f %f %f)\n", cameraPos.x - gw2_data->fAvatarPosition[0],
					   cameraPos.y - gw2_data->fAvatarPosition[1], cameraPos.z - gw2_data->fAvatarPosition[2]);
				printf("Pos (%f %f %f)\n", cameraPos.x, cameraPos.y, cameraPos.z);
				printf("Front (%f %f %f)\n", cameraFront.x, cameraFront.y, cameraFront.z);
				printf("Map rect ((%f %f),(%f %f))\n", map.get_map_rect(0, 0), map.get_map_rect(0, 1),
		   map.get_map_rect(1, 0), map.get_map_rect(1, 1)); printf("Cont rect ((%f %f),(%f %f))\n",
		   map.get_continent_rect(0, 0), map.get_continent_rect(0, 1), map.get_continent_rect(1, 0),
		   map.get_continent_rect(1, 1));
					   */

	} else {
		glm::vec3 cameraPos =
			glm::vec3(gw2_data->fCameraPosition[0], gw2_data->fCameraPosition[1], gw2_data->fCameraPosition[2]);
		glm::vec3 cameraFront =
			glm::vec3(gw2_data->fCameraFront[0], gw2_data->fCameraFront[1], gw2_data->fCameraFront[2]);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		// XXX: seems like I didn't understand this part :) doesn't change anything
		// glm::vec3 cameraTarget = glm::make_vec3(gw2_data->fAvatarPosition);
		// glm::vec3 cameraDirection = glm::normalize(cameraPos - glm::make_vec3(cameraTarget));
		// glm::vec3 cameraRight = glm::normalize(glm::cross(cameraUp, cameraDirection));
		// cameraUp = glm::cross(cameraDirection, cameraRight);
		// printf("New up (%f %f %f)\n", cameraUp.x, cameraUp.y, cameraUp.z);
		// printf("Pos (%f %f %f)\n", cameraPos.x, cameraPos.y, cameraPos.z);
		// printf("Front (%f %f %f)\n", cameraFront.x, cameraFront.y, cameraFront.z);

		auto view = glm::lookAtLH(cameraPos, cameraPos + cameraFront, cameraUp);
		my_shader->set_view(view);
	}
}

void update_map(int id) {
	auto d = api.get_value(std::string("v2/maps/") + std::to_string(id));
	map.load_map(d);
}

int main(int argc, char** argv) {
	po::options_description desc("Allowed options");
	// clang-format off
	desc.add_options()
		("help,h", "show usage")
		("xml", po::value<std::vector<std::string>>(),
			"Path to xml file")
		("xml-folder", po::value<std::vector<std::string>>(),
			"Path to folder with xml files")
		("width,w", po::value<float>()->default_value(1680.0f),
			"Display width")
		("height", po::value<float>()->default_value(1050.0f),
			"Display height")
		("vulkan", "Enable vulkan renderer")
		("validation", "Enable validation layers for vulkan")
		("f", "Force output")
		;
	// clang-format on
	po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
	} catch (std::exception& e) {
		std::cerr << e.what();
		std::cout << desc << std::endl;
		return 1;
	}

	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 0;
	}
	std::vector<std::string> xml_files;
	if (vm.count("xml")) {
		xml_files = vm["xml"].as<std::vector<std::string>>();
	}
	if (vm.count("xml-folder")) {
		auto folder_vec = vm["xml-folder"].as<std::vector<std::string>>();
		for (auto iter = folder_vec.begin(); iter != folder_vec.end(); ++iter) {
			for (const auto& entry : std::filesystem::directory_iterator(*iter)) {
				if (entry.is_regular_file() && entry.path().extension() == ".xml") {
					xml_files.push_back(entry.path().string());
				}
			}
		}
	}

	float screenWidth = vm["width"].as<float>();
	float screenHeight = vm["height"].as<float>();

	bool use_vulkan = vm.count("vulkan");

	WindowData window = createTransparentWindow("GAME", 1280, 0, screenWidth, screenHeight, !use_vulkan);
	std::shared_ptr<Renderer> renderer;

	if (use_vulkan) {
		renderer.reset(new VKRenderer(window, vm.count("validation")));
	} else {
		renderer.reset(new GLRenderer(window));
	}

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		std::cerr << "Failed to init SDL_Image!" << std::endl;
		return 1;
	}

	const char* vertex_shader_src =
		"#version 420 core\n"
		"layout (location = 0) in vec3 aPos;"
		"layout (location = 0) in vec3 aColor;"
		"layout (location = 2) in vec2 aTexCoord;"
		"out vec3 ourColor;"
		"out vec2 TexCoord;"
		"uniform mat4 transform;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main() {"
		"ourColor = aColor;"
		"TexCoord = aTexCoord;"
		"TexCoord.y *= -1;"
		"gl_Position = projection * (view*transform * vec4(0.0, 0.0, 0.0, "
		"1.0) "
		"+ vec4(aPos.x, aPos.y, 0.0, 0.0));"
		"}\0";

	const char* fragment_shader_src =
		"#version 420 core\n"
		"out vec4 FragColor;\n"
		"in vec3 ourColor;"
		"in vec2 TexCoord;"
		"uniform sampler2D ourTexture;"
		"void main() {\n"
		//"FragColor = texture(ourTexture, TexCoord) *
		// vec4(ourColor, 1.0f);"
		"FragColor = texture(ourTexture, TexCoord);"
		"}";

	my_shader = renderer->load_shader("", "");

	my_shader->load_from_string(vertex_shader_src, fragment_shader_src);
	my_shader->set_active();

	CategoryManager::getInstance().load_taco_xmls(xml_files);

	GW2Link gw_link;
	auto gw2_data = gw_link.get_gw2_data();

	bool running = true;
	printf("Starting main loop\n");
	uint64_t last_call = 0;

	int last_id = 0;
	{
		gw_link.update_gw2(true);
		std::string identity = gw2_data->get_identity();
		float fov = 1.222f;
		try {
			auto json = json::parse(identity);
			std::cout << json.dump() << std::endl;
			fov = json["fov"];
		} catch (...) {
		}
		my_shader->set_projection(fov, screenWidth, screenHeight);
	}

	std::thread qt_thread([&]() { qt_main(argc, argv); });
	while (running) {
		uint64_t delta = SDL_GetTicks() - last_call;
		gw_link.update_gw2(true);
		auto ctx = gw2_data->get_context();
		if (ctx->mapId != last_id) {
			printf("Map id changed to %d\n", ctx->mapId);
			last_id = ctx->mapId;
			CategoryManager::getInstance().set_state_changed(true);
			update_map(last_id);
		}
		if (CategoryManager::getInstance().state_changed()) {
			load_objects(last_id, renderer);
			renderer->set_objects(objects);
			CategoryManager::getInstance().set_state_changed(false);
		}

		if (ctx->get_ui_state(UI_STATE::GAME_FOCUS) || vm.count("f")) {
			update_camera(gw2_data);
			renderer->update();
		} else {
			renderer->clear();
		}
	}

	return 0;
}
