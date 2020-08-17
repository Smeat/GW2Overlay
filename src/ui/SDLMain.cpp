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
 *    - Currently GW2 needs to be started and then start the mumble script via the wine cmd
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
#include <SDL2/SDL_timer.h> 
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_syswm.h>

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
#include <memory>
#include <sstream>
#include <thread>
#include <iostream>
#include <unordered_map>
#include <functional>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xlib-xcb.h>
#include <vector>
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xfixes.h>
#include <xcb/shape.h>
#include <xcb/xinput.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <locale.h>
#include <wchar.h>
#include <string.h>

#include "Shader.h"
#include "Object.h"
#include "Mesh.h"

#include "../utils/xml/pugixml.hpp"
#include "../utils/json/json.hpp"
#include "Texture.h"

using json = nlohmann::json;

struct __attribute__((packed)) MumbleContext {
	unsigned char serverAddress[28]; // contains sockaddr_in or sockaddr_in6
	uint32_t mapId;
	uint32_t mapType;
	uint32_t shardId;
	uint32_t instance;
	uint32_t buildId;
	// Additional data beyond the 48 bytes Mumble uses for identification
	uint32_t uiState; // Bitmask: Bit 1 = IsMapOpen, Bit 2 = IsCompassTopRight, Bit 3 = DoesCompassHaveRotationEnabled, Bit 4 = Game has focus, Bit 5 = Is in Competitive game mode, Bit 6 = Textbox has focus, Bit 7 = Is in Combat
	uint16_t compassWidth; // pixels
	uint16_t compassHeight; // pixels
	float compassRotation; // radians
	float playerX; // continentCoords
	float playerY; // continentCoords
	float mapCenterX; // continentCoords
	float mapCenterY; // continentCoords
	float mapScale;
	uint32_t processId;
	uint8_t mountIndex;
};

struct __attribute__((packed)) LinkedMem{
	uint32_t uiVersion;
	uint32_t uiTick;
	float fAvatarPosition[3];
	float fAvatarFront[3];
	float fAvatarTop[3];
	uint16_t name[256];
	float fCameraPosition[3];
	float fCameraFront[3];
	float fCameraTop[3];
	uint16_t identity[256];
	uint32_t context_len; // Despite the actual context containing more data, this value is always 48. See "context" section below.
	unsigned char context[256];
//	wchar_t description[2048]; // always empty
};

// TODO: remove this global mess and create more files
Window        mXWindow;
Display*      mXDisplay;
int socket_fd = -1;
LinkedMem gw2_data;
std::vector<std::shared_ptr<Object>> objects;
std::shared_ptr<Shader> my_shader(new Shader);

void load_xml(const std::string& filename, int map_id) {
	std::unordered_map<std::string, std::string> type_file_map;
	std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename.c_str());
	// load textures

	std::function<void(const std::string&, pugi::xml_node)> traverse_func;
	traverse_func = [&](const std::string& name, pugi::xml_node node) {
			for(pugi::xml_node n: node.children()) {
				std::string new_name = name.empty() ? n.attribute("name").value() : name + "." + n.attribute("name").value();
				traverse_func(new_name, n);
			}
		if(node.name() == std::string("MarkerCategory")) {
			if(!node.attribute("iconFile").empty()) {
				type_file_map[name] = node.attribute("iconFile").value();
	//			std::cout << "Adding " << name << " : " << node.attribute("iconFile").value() << std::endl;
			}
		}
		return "";
	};

	traverse_func("", doc.child("OverlayData"));

	std::vector<Vertex> vertices;
	vertices.push_back(Vertex(glm::vec3(0.5f,  0.5f, 0.0f),glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f,1.0f)));
	vertices.push_back(Vertex(glm::vec3(0.5f, -0.5f, 0.0f),glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)));
	vertices.push_back(Vertex(glm::vec3(-0.5f, -0.5f, 0.0f),glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)));
	vertices.push_back(Vertex(glm::vec3(-0.5f,  0.5f, 0.0f),glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)));
	objects.clear();
	for(auto poi = doc.child("OverlayData").child("POIs").child("POI"); poi; poi = poi.next_sibling("POI")) {
		if(poi.attribute("MapID").as_int() == map_id) {
			//std::cout << "POI: " << poi.attribute("type").value() << std::endl;
			glm::vec3 pos(poi.attribute("xpos").as_float(), poi.attribute("ypos").as_float(),poi.attribute("zpos").as_float());
			std::string poi_name = poi.attribute("type").value();
			auto filename = type_file_map.find(poi_name);
			if(filename == type_file_map.end()) {
				std::cout << "Couldn't find " << poi_name << std::endl;
				continue;
			}
			if(textures.find(filename->second) == textures.end()) {
	//			std::cout << "loading texture " << filename->second << std::endl;
				std::shared_ptr<Texture> tex(new Texture(filename->second));
				textures.insert({filename->second, tex});
			}
			auto tex_iter = textures.find(filename->second);
			Mesh my_mesh(vertices, {0,1,3,1,2,3},tex_iter->second);
			std::shared_ptr<Object> obj(new Object(my_shader, {my_mesh}));
			obj->translate(pos);
			obj->scale({3.0f, 3.0f, 1.0f});
			objects.push_back(obj);
		}
	}
}

int create_socket() {
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd == -1) {
		printf("Failed to create socket\n");
		return fd;
	}
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	// Filling server information
	servaddr.sin_family    = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(7070);
	
	// TODO: proper getaddr
	if(::bind(fd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
		printf("Failed to bind socket\n");
		::close(fd);
		fd = -1;
	}

	printf("Created socket\n");
	return fd;
}

void update_gw2(bool block = false){
	struct sockaddr_in si_other;
	int len = 0, slen = sizeof(si_other);
	LinkedMem data;
	int flags = 0;
	if(!block) flags |= MSG_DONTWAIT;
	if((len = ::recvfrom(socket_fd, &data, sizeof(data), MSG_DONTWAIT, (struct sockaddr *) &si_other, (socklen_t *)&slen)) > 0){
		float* debug;
		debug = data.fAvatarPosition;
		/*printf("Got data %f %f %f\n", debug[0], debug[1], debug[2]);
		printf("uiVersion %d, uiTick %d\n", data.uiVersion, data.uiTick);
		printf("fAvatarPosition (%f %f %f) fAvatarFront (%f %f %f) fAvatarTop (%f %f %f)\n", data.fAvatarPosition[0], data.fAvatarPosition[1], data.fAvatarPosition[2], data.fAvatarFront[0], data.fAvatarFront[1], data.fAvatarFront[2], data.fAvatarTop[0], data.fAvatarTop[1], data.fAvatarTop[2]);
//		std::wcout << data.name << std::endl;
//		std::wcout << data.identity << std::endl;
		printf("fCameraPosition (%f %f %f) fCameraFront (%f %f %f) fCameraTop (%f %f %f)\n", data.fCameraPosition[0], data.fCameraPosition[1], data.fCameraPosition[2], data.fCameraFront[0], data.fCameraFront[1], data.fCameraFront[2], data.fCameraTop[0], data.fCameraTop[1], data.fCameraTop[2]);
		printf("Contextlen %d\n", data.context_len);
		printf("Packet len %d should be %lu, diff %d ctx %lu\n", len, sizeof(LinkedMem), int(sizeof(LinkedMem) - len), sizeof(MumbleContext));
		*/
		gw2_data = data;
	}
}

SDL_Window* SDL_CreateTransparentWindow(const char* title, int x, int y, int w, int h)
{
	mXDisplay = XOpenDisplay(0);

	if (mXDisplay == 0) {
		printf("Failed to connect to the Xserver\n");
		return NULL;
	}
	xcb_connection_t* xconn = XGetXCBConnection(mXDisplay);

	XVisualInfo visualinfo;
	XMatchVisualInfo(mXDisplay, DefaultScreen(mXDisplay), 32, TrueColor, &visualinfo);

	GC gc;
	XSetWindowAttributes attr;
	attr.colormap   = XCreateColormap(mXDisplay, DefaultRootWindow(mXDisplay), visualinfo.visual, AllocNone);
	attr.event_mask = NoEventMask;
	attr.background_pixmap = None;
	attr.border_pixel = 0;

	mXWindow = XCreateWindow(mXDisplay, DefaultRootWindow(mXDisplay),
						x, y, w, h, // x,y,width,height : are possibly opverwriteen by window manager
						0,
						visualinfo.depth,
						InputOutput,
						visualinfo.visual,
						CWColormap|CWEventMask|CWBackPixmap|CWBorderPixel,
						&attr);
	gc = XCreateGC(mXDisplay, mXWindow, 0, 0);
	printf("Window has id: %lu\n", mXWindow);

	int baseEventMask
        = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY
            | XCB_EVENT_MASK_PROPERTY_CHANGE | XCB_EVENT_MASK_FOCUS_CHANGE;
	int transparentForInputEventMask = baseEventMask
            | XCB_EVENT_MASK_VISIBILITY_CHANGE | XCB_EVENT_MASK_RESIZE_REDIRECT
            | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT
            | XCB_EVENT_MASK_COLOR_MAP_CHANGE | XCB_EVENT_MASK_OWNER_GRAB_BUTTON;
	const int mask = XCB_CW_OVERRIDE_REDIRECT | XCB_CW_EVENT_MASK;
	const int values[] = {1, transparentForInputEventMask};
	xcb_void_cookie_t cookie = xcb_change_window_attributes_checked(xconn, mXWindow, mask, values);
	xcb_generic_error_t *error;
	if ((error = xcb_request_check(xconn, cookie))) {
		fprintf(stderr, "Could not reparent the window\n");
		free(error);
		return NULL;
	} else {
		printf("Changed attributes\n");
	}
	// Mouse passthrough
	// init xfixes
	const xcb_query_extension_reply_t *reply = xcb_get_extension_data(xconn, &xcb_xfixes_id);
	if (!reply || !reply->present){
		return NULL;
	}

	auto xfixes_query = xcb_xfixes_query_version(xconn,
                                    XCB_XFIXES_MAJOR_VERSION,
                                    XCB_XFIXES_MINOR_VERSION);
	auto xfixesQuery = xcb_xfixes_query_version_reply(xconn, xfixes_query, NULL);
	if (!xfixesQuery || xfixesQuery->major_version < 2) {
		printf("failed to initialize XFixes\n");
		return NULL;
	}
	
	xcb_rectangle_t rectangle;

	xcb_rectangle_t *rect = nullptr;
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
	xcb_xfixes_set_window_shape_region_checked(xconn, mXWindow, XCB_SHAPE_SK_INPUT, 0, 0, region);
	xcb_xfixes_destroy_region(xconn, region);

	GLXContext glcontext = glXCreateContext(mXDisplay, &visualinfo, 0, True) ;
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

void update_gl(int delta){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for(auto iter = objects.begin(); iter != objects.end(); ++iter) {
		(*iter)->update();
	}
	glFlush();
}

void set_projection(float fov_rad, float w, float h, float near = 0.1f, float far = 1000.0f) {
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspectiveFovLH(fov_rad, w, h, near, far);
	my_shader->set_mat4("projection", projection);
}

int main(int argc, char** argv) {
	float screenWidth = 1680.0f;
	float screenHeight = 1050.0f;
	if (SDL_Init(SDL_INIT_VIDEO) != 0) { 
		printf("error initializing SDL: %s\n", SDL_GetError()); 
	}
	SDL_Window* window = SDL_CreateTransparentWindow("GAME", 1280, 0, screenWidth, screenHeight);

	int imgFlags = IMG_INIT_PNG;
	if(!(IMG_Init(imgFlags) & imgFlags)) {
		std::cerr << "Failed to init SDL_Image!" << std::endl;
		return 1;
	}

	glewInit();

	printf("OpenGL version %s\n", glGetString(GL_VERSION));

	const char *vertex_shader_src = "#version 420 core\n"
	"layout (location = 0) in vec3 aPos;"
	"layout (location = 0) in vec3 aColor;"
	"layout (location = 2) in vec2 aTexCoord;"
	"out vec3 ourColor;"
	"out vec2 TexCoord;"
	"uniform mat4 transform;"
	"uniform mat4 view;"
	"uniform mat4 projection;"
	"void main() {"
    "   gl_Position = projection * view * transform * vec4(aPos, 1.0f);"
	"   ourColor = aColor;"
	"   TexCoord = aTexCoord;"
	"TexCoord.y *= -1;"
	"gl_Position = projection * (view*transform * vec4(0.0, 0.0, 0.0, 1.0) + vec4(aPos.x, aPos.y, 0.0, 0.0));"
	"}\0";

	const char *fragment_shader_src = "#version 420 core\n"
	"out vec4 FragColor;\n"
	"in vec3 ourColor;"
	"in vec2 TexCoord;"
	"uniform sampler2D ourTexture;"
	"void main() {\n"
	//"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	//"FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0f);"
	"FragColor = texture(ourTexture, TexCoord);"
	"}";

	glm::mat4 model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	glm::mat4 view          = glm::mat4(1.0f);

	my_shader->load_from_string(vertex_shader_src, fragment_shader_src);
	my_shader->set_active();
	my_shader->set_mat4("transform", model);
	my_shader->set_mat4("view", view);

	socket_fd = create_socket();

	bool running = true;
	printf("Starting main loop\n");
	uint64_t last_call = 0;

	int last_id = 0;
	load_xml("/tmp/test.xml", 50);
	{
		update_gw2(true);
		MumbleContext* ctx = (MumbleContext*)gw2_data.context;
		char c = gw2_data.identity[0];
		int i = 0;
		std::stringstream ss;
		while(c != 0 && i < 256) {
			i += 1;
			ss << c;
			c = gw2_data.identity[i];
		}
		float fov = 1.222f;
		try {
			auto json = json::parse(ss.str());
			std::cout << json.dump() << std::endl;
			fov = json["fov"];
		}
		catch(...) {
		}
		set_projection(fov, screenWidth, screenHeight);
	}

	while(running) {
		uint64_t delta = SDL_GetTicks() - last_call;
		update_gw2(true);
		MumbleContext* ctx = (MumbleContext*)gw2_data.context;
//		printf("Mount index %d size %lu space %lu\n", ctx->mountIndex, sizeof(MumbleContext), sizeof(gw2_data.context));
		if(ctx->mapId != last_id) {
			printf("Map id changed to %d\n", ctx->mapId);
			last_id = ctx->mapId;
			load_xml("/tmp/test.xml", last_id);
		}

		glm::vec3 cameraPos   = glm::make_vec3(gw2_data.fCameraPosition);
		glm::vec3 cameraFront = glm::make_vec3(gw2_data.fCameraFront);
		glm::vec3 cameraUp    = glm::make_vec3(gw2_data.fCameraTop);
		// TODO: proper camera UP! Any way to get the target? Mumble link cam up is always 0
		// this is good enough for now
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		glm::vec3 cameraTarget = glm::make_vec3(gw2_data.fAvatarPosition);
		glm::vec3 cameraDirection = glm::normalize(cameraPos - glm::make_vec3(cameraTarget));
		glm::vec3 cameraRight = glm::normalize(glm::cross(cameraUp, cameraDirection));

		view = glm::lookAtLH(cameraPos, cameraPos + cameraFront, cameraUp);
		my_shader->set_mat4("view", view);
		if((ctx->uiState & (1 << 3))) {
			update_gl(delta);
		} else {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		glXSwapBuffers(mXDisplay, mXWindow);
		SDL_Event event;

		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:
					running = false;
					break;
			}
		}
	//	SDL_Delay(1000 / 100);
	}
	return 0;
} 
