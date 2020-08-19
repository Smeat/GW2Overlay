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
 * FIXME:
 *  - Objects are moving a little bit
 *   - Probably due to socket delay
 */

#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

#include <GL/gl.h>
#include <GL/glx.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <X11/X.h>
#include <X11/Xutil.h>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <thread>
#include <unordered_map>

#include <X11/Xatom.h>
#include <X11/Xlib-xcb.h>
#include <X11/Xlib.h>
#include <xcb/shape.h>
#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xfixes.h>
#include <xcb/xinput.h>
#include <xcb/xproto.h>
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
#include "../utils/GW2Link.h"
#include "../utils/POI.h"
#include "../utils/json/json.hpp"
#include "../utils/xml/pugixml.hpp"
#include "QtMain.h"
#include "Texture.h"

using json = nlohmann::json;
namespace po = boost::program_options;

// TODO: remove this global mess and create more files
Window mXWindow;
Display* mXDisplay;
std::vector<std::shared_ptr<Object>> objects;
std::shared_ptr<Shader> my_shader(new Shader);

void load_objects(int mapid) {
	std::vector<Vertex> vertices;
	vertices.push_back(Vertex(glm::vec3(0.5f, 0.5f, 0.0f),
							  glm::vec3(1.0f, 0.0f, 0.0f),
							  glm::vec2(1.0f, 1.0f)));
	vertices.push_back(Vertex(glm::vec3(0.5f, -0.5f, 0.0f),
							  glm::vec3(0.0f, 1.0f, 0.0f),
							  glm::vec2(1.0f, 0.0f)));
	vertices.push_back(Vertex(glm::vec3(-0.5f, -0.5f, 0.0f),
							  glm::vec3(0.0f, 0.0f, 1.0f),
							  glm::vec2(0.0f, 0.0f)));
	vertices.push_back(Vertex(glm::vec3(-0.5f, 0.5f, 0.0f),
							  glm::vec3(1.0f, 1.0f, 0.0f),
							  glm::vec2(0.0f, 1.0f)));
	objects.clear();
	std::unordered_map<std::string, std::shared_ptr<Texture>> texture_file_map;

	auto markers = CategoryManager::getInstance().get_categories();
	auto pois = CategoryManager::getInstance().get_pois();

	for (auto iter = pois->begin(); iter != pois->end(); ++iter) {
		if (iter->m_map_id == mapid) {
			auto cat = MarkerCategory::find_children(*markers, iter->m_type);
			if (!cat) {
				std::cout << "Couldn't find " << iter->m_type << std::endl;
				continue;
			}
			if (texture_file_map.find(cat->m_icon_file) ==
				texture_file_map.end()) {
				std::shared_ptr<Texture> tex(new Texture(cat->m_icon_file));
				texture_file_map.insert({cat->m_icon_file, tex});
			}
			auto tex_iter = texture_file_map.find(cat->m_icon_file);
			std::shared_ptr<Mesh> my_mesh(
				new Mesh(vertices, {0, 1, 3, 1, 2, 3}, tex_iter->second));
			std::shared_ptr<Object> obj(new Object(my_shader, {my_mesh}));
			auto pos = iter->m_pos;
			pos.y += cat->m_height_offset;
			obj->translate(pos);
			obj->scale(
				{cat->m_icon_size * 3.0f, cat->m_icon_size * 3.0f, 1.0f});
			objects.push_back(obj);
		}
	}
}

SDL_Window* SDL_CreateTransparentWindow(const char* title, int x, int y, int w,
										int h) {
	mXDisplay = XOpenDisplay(0);

	if (mXDisplay == 0) {
		printf("Failed to connect to the Xserver\n");
		return NULL;
	}
	xcb_connection_t* xconn = XGetXCBConnection(mXDisplay);

	XVisualInfo visualinfo;
	XMatchVisualInfo(mXDisplay, DefaultScreen(mXDisplay), 32, TrueColor,
					 &visualinfo);

	GC gc;
	XSetWindowAttributes attr;
	attr.colormap = XCreateColormap(mXDisplay, DefaultRootWindow(mXDisplay),
									visualinfo.visual, AllocNone);
	attr.event_mask = NoEventMask;
	attr.background_pixmap = None;
	attr.border_pixel = 0;

	mXWindow = XCreateWindow(
		mXDisplay, DefaultRootWindow(mXDisplay), x, y, w,
		h,	// x,y,width,height : are possibly opverwriteen by window manager
		0, visualinfo.depth, InputOutput, visualinfo.visual,
		CWColormap | CWEventMask | CWBackPixmap | CWBorderPixel, &attr);
	gc = XCreateGC(mXDisplay, mXWindow, 0, 0);
	printf("Window has id: %lu\n", mXWindow);

	int baseEventMask =
		XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY |
		XCB_EVENT_MASK_PROPERTY_CHANGE | XCB_EVENT_MASK_FOCUS_CHANGE;
	int transparentForInputEventMask =
		baseEventMask | XCB_EVENT_MASK_VISIBILITY_CHANGE |
		XCB_EVENT_MASK_RESIZE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
		XCB_EVENT_MASK_COLOR_MAP_CHANGE | XCB_EVENT_MASK_OWNER_GRAB_BUTTON;
	const int mask = XCB_CW_OVERRIDE_REDIRECT | XCB_CW_EVENT_MASK;
	const int values[] = {1, transparentForInputEventMask};
	xcb_void_cookie_t cookie =
		xcb_change_window_attributes_checked(xconn, mXWindow, mask, values);
	xcb_generic_error_t* error;
	if ((error = xcb_request_check(xconn, cookie))) {
		fprintf(stderr, "Could not reparent the window\n");
		free(error);
		return NULL;
	} else {
		printf("Changed attributes\n");
	}
	// Mouse passthrough
	// init xfixes
	const xcb_query_extension_reply_t* reply =
		xcb_get_extension_data(xconn, &xcb_xfixes_id);
	if (!reply || !reply->present) {
		return NULL;
	}

	auto xfixes_query = xcb_xfixes_query_version(
		xconn, XCB_XFIXES_MAJOR_VERSION, XCB_XFIXES_MINOR_VERSION);
	auto xfixesQuery =
		xcb_xfixes_query_version_reply(xconn, xfixes_query, NULL);
	if (!xfixesQuery || xfixesQuery->major_version < 2) {
		printf("failed to initialize XFixes\n");
		return NULL;
	}

	xcb_rectangle_t rectangle;

	xcb_rectangle_t* rect = nullptr;
	int nrect = 0;

	int offset = w;
	rectangle.x = offset;
	rectangle.y = 0;
	rectangle.width = w + offset;
	rectangle.height = h;
	rect = &rectangle;
	nrect = 1;

	xcb_xfixes_region_t region = xcb_generate_id(xconn);
	xcb_xfixes_create_region(xconn, region, nrect, rect);
	xcb_xfixes_set_window_shape_region_checked(
		xconn, mXWindow, XCB_SHAPE_SK_INPUT, 0, 0, region);
	xcb_xfixes_destroy_region(xconn, region);

	GLXContext glcontext = glXCreateContext(mXDisplay, &visualinfo, 0, True);
	if (!glcontext) {
		printf("Failed to create openGL context\n");
		return NULL;
	}
	glXMakeCurrent(mXDisplay, mXWindow, glcontext);

	XMapWindow(mXDisplay, mXWindow);
	glXSwapBuffers(mXDisplay, mXWindow);
	SDL_Window* sdl_window = SDL_CreateWindowFrom((void*)mXWindow);
	return sdl_window;
}

void update_gl(int delta) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (auto iter = objects.begin(); iter != objects.end(); ++iter) {
		(*iter)->update();
	}
	glFlush();
}

void set_projection(float fov_rad, float w, float h, float near = 0.1f,
					float far = 1000.0f) {
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspectiveFovLH(fov_rad, w, h, near, far);
	my_shader->set_mat4("projection", projection);
}

void update_camera(const LinkedMem* gw2_data) {
	//	auto ctx = gw2_data->get_context();
	//	TODO: draw POIs on the map, if it is open
	//	This requires to translate the position of all objects to map
	// coordinates
	glm::vec3 cameraPos = glm::make_vec3(gw2_data->fCameraPosition);
	glm::vec3 cameraFront = glm::make_vec3(gw2_data->fCameraFront);
	glm::vec3 cameraUp = glm::make_vec3(gw2_data->fCameraTop);
	// TODO: proper camera UP! Any way to get the target? Mumble link cam up
	// is always 0 this is good enough for now
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 cameraTarget = glm::make_vec3(gw2_data->fAvatarPosition);
	glm::vec3 cameraDirection =
		glm::normalize(cameraPos - glm::make_vec3(cameraTarget));
	glm::vec3 cameraRight =
		glm::normalize(glm::cross(cameraUp, cameraDirection));

	auto view = glm::lookAtLH(cameraPos, cameraPos + cameraFront, cameraUp);
	my_shader->set_mat4("view", view);
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
			for (const auto& entry :
				 std::filesystem::directory_iterator(*iter)) {
				if (entry.is_regular_file() &&
					entry.path().extension() == ".xml") {
					xml_files.push_back(entry.path().string());
				}
			}
		}
	}

	float screenWidth = vm["width"].as<float>();
	float screenHeight = vm["height"].as<float>();
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("error initializing SDL: %s\n", SDL_GetError());
	}
	SDL_Window* window =
		SDL_CreateTransparentWindow("GAME", 1280, 0, screenWidth, screenHeight);

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		std::cerr << "Failed to init SDL_Image!" << std::endl;
		return 1;
	}

	glewInit();

	printf("OpenGL version %s\n", glGetString(GL_VERSION));
	glEnable(GL_DEPTH_TEST);
	glAlphaFunc(GL_GREATER, 0.5f);
	glEnable(GL_ALPHA_TEST);

	// TODO: fix blending. since almost all objects have transparency, they all
	// need to be sorted and drawn in the correct order is that worth it?
	// glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);

	my_shader->load_from_string(vertex_shader_src, fragment_shader_src);
	my_shader->set_active();
	my_shader->set_mat4("transform", model);
	my_shader->set_mat4("view", view);

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
		set_projection(fov, screenWidth, screenHeight);
	}

	std::thread qt_thread([&]() { qt_main(argc, argv); });
	while (running) {
		uint64_t delta = SDL_GetTicks() - last_call;
		gw_link.update_gw2(true);
		auto ctx = gw2_data->get_context();
		if (ctx->mapId != last_id) {
			printf("Map id changed to %d\n", ctx->mapId);
			last_id = ctx->mapId;
			// TODO: load other objects into gl
			load_objects(last_id);
		}

		if (ctx->get_ui_state(UI_STATE::GAME_FOCUS)) {
			update_camera(gw2_data);
			update_gl(delta);
		} else {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		glXSwapBuffers(mXDisplay, mXWindow);
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					running = false;
					break;
			}
		}
		// SDL_Delay(1000 / 100);
	}

	return 0;
}
