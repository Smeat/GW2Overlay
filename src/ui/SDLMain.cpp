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
#include <GL/glut.h>

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
#include <thread>
#include <iostream>

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

struct MumbleContext {
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


Window        mXWindow;
Display*      mXDisplay;
int socket_fd = -1;
LinkedMem gw2_data;
std::vector<std::shared_ptr<Object>> objects;

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

void update_gw2(){
	struct sockaddr_in si_other;
	int len = 0, slen = sizeof(si_other);
	LinkedMem data;
	if((len = ::recvfrom(socket_fd, &data, sizeof(data), MSG_DONTWAIT, (struct sockaddr *) &si_other, (socklen_t *)&slen)) > 0){
		float* debug;
		debug = data.fAvatarPosition;
		printf("Got data %f %f %f\n", debug[0], debug[1], debug[2]);
		printf("uiVersion %d, uiTick %d\n", data.uiVersion, data.uiTick);
		printf("fAvatarPosition (%f %f %f) fAvatarFront (%f %f %f) fAvatarTop (%f %f %f)\n", data.fAvatarPosition[0], data.fAvatarPosition[1], data.fAvatarPosition[2], data.fAvatarFront[0], data.fAvatarFront[1], data.fAvatarFront[2], data.fAvatarTop[0], data.fAvatarTop[1], data.fAvatarTop[2]);
//		std::wcout << data.name << std::endl;
//		std::wcout << data.identity << std::endl;
		printf("fCameraPosition (%f %f %f) fCameraFront (%f %f %f) fCameraTop (%f %f %f)\n", data.fCameraPosition[0], data.fCameraPosition[1], data.fCameraPosition[2], data.fCameraFront[0], data.fCameraFront[1], data.fCameraFront[2], data.fCameraTop[0], data.fCameraTop[1], data.fCameraTop[2]);
		printf("Contextlen %d\n", data.context_len);
		printf("Packet len %d should be %lu\n", len, sizeof(LinkedMem));
		printf("Size wchar_t %lu\n", sizeof(wchar_t));
		gw2_data = data;
	}
}

SDL_Window* SDL_CreateTransparentWindow(const char* title, int x, int y, int w, int h)
{
	/*Window mXWindow;*/
	/*Display*/ mXDisplay = XOpenDisplay(0);
	const char* xserver = getenv("DISPLAY");

	if (mXDisplay == 0)
	{
		printf("Could not establish a connection to X-server '%s'\n", xserver );
		exit(1) ;
	}
	xcb_connection_t* xconn = XGetXCBConnection(mXDisplay);


	// query Visual for "TrueColor" and 32 bits depth (RGBA)
	XVisualInfo visualinfo;
	XMatchVisualInfo(mXDisplay, DefaultScreen(mXDisplay), 32, TrueColor, &visualinfo);

	// create window
	GC gc;
	XSetWindowAttributes attr;
	attr.colormap   = XCreateColormap(mXDisplay, DefaultRootWindow(mXDisplay), visualinfo.visual, AllocNone);
	//attr.event_mask = ExposureMask;// | KeyPressMask;
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

	// set title bar name of window
	XStoreName(mXDisplay, mXWindow, title);

	// say window manager which position we would prefer
	XSizeHints sizehints ;
	sizehints.flags = PPosition | PSize;
	sizehints.x	 = x ;  sizehints.y = y;
	sizehints.width = w ; sizehints.height = h;
	XSetWMNormalHints(mXDisplay, mXWindow, &sizehints);

	// Switch On >> If user pressed close key let window manager only send notification >>
	Atom wm_delete_window = XInternAtom(mXDisplay, "WM_DELETE_WINDOW", 0);
	XSetWMProtocols(mXDisplay, mXWindow, &wm_delete_window, 1);

	// Disable input
	XWMHints* hints = XAllocWMHints();
	hints->input = false;
	hints->flags = InputHint;
	//XSetWMHints(mXDisplay, mXWindow, hints);
	
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
    if (!reply || !reply->present)
        return NULL;

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

	int offset = 1680;
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


	// create OpenGL context
	GLXContext glcontext = glXCreateContext(mXDisplay, &visualinfo, 0, True) ;

	if (!glcontext)
	{
		printf("X11 server '%s' does not support OpenGL\n", xserver ) ;
		return NULL;
	}

	glXMakeCurrent(mXDisplay, mXWindow, glcontext);

	// now let the window appear to the userSDL_WINDOWPOS_U
	XMapWindow(mXDisplay, mXWindow);
	glXSwapBuffers(mXDisplay, mXWindow);
	

	SDL_Window* sdl_window = SDL_CreateWindowFrom((void*)mXWindow);
	SDL_Delay(30);
	return sdl_window;
}


void drawSnowMan() {

	glColor3f(1.0f, 1.0f, 1.0f);

// Draw Body
	glTranslatef(0.0f ,0.75f, 0.0f);
	glutSolidSphere(0.75f,20,20);

// Draw Head
	glTranslatef(0.0f, 1.0f, 0.0f);
	glutSolidSphere(0.25f,20,20);

// Draw Eyes
	glPushMatrix();
	glColor3f(0.0f,0.0f,0.0f);
	glTranslatef(0.05f, 0.10f, 0.18f);
	glutSolidSphere(0.05f,10,10);
	glTranslatef(-0.1f, 0.0f, 0.0f);
	glutSolidSphere(0.05f,10,10);
	glPopMatrix();

// Draw Nose
	glColor3f(1.0f, 0.5f , 0.5f);
	glutSolidCone(0.08f,0.5f,10,2);
}

void update_gl(int delta){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
/*	glBegin(GL_POLYGON);
	static int val = 0;
	val += 1;
	val = val % 50;
	//glVertex3f(0.0, 0.0, 0.0);
	//glVertex3f(0.5, 0.0, 0.0);
	//glVertex3f(0.5, 0.5, 0.0);
	//glVertex3f(0.0, 0.5, 0.0);
	glVertex3f(0.5f,  0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glVertex3f(-0.5f,  0.5f, 0.0f);
	glEnd();*/
	//add_object(-0.5,-0.5,0);
	for(auto iter = objects.begin(); iter != objects.end(); ++iter) {
		(*iter)->update();
	}

	//drawSnowMan();
	glFlush();
}



int main(int argc, char** argv) {
	float screenWidth = 1680;
	float screenHeight = 1050;
	// retutns zero on success else non-zero 
	if (SDL_Init(SDL_INIT_VIDEO) != 0) { 
		printf("error initializing SDL: %s\n", SDL_GetError()); 
	} 
	SDL_Window* window = SDL_CreateTransparentWindow("GAME", 1280, 0, screenWidth, screenHeight);
	//int window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP;
	//SDL_Window* window = SDL_CreateWindow("GAME", 1280, 0, 1680, 1050, window_flags);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
	glutInit(&argc, argv);
	glewInit();

	/*SDL_GLContext glcontext = SDL_GL_CreateContext(window);
	if(glcontext == NULL) {
		printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
		return 1;
	}
	*/
	//SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	//SDL_RenderClear(renderer);
//	SDL_GL_SetSwapInterval(1);
//	glClearColor( 0, 0.5, 0, 0.5) ; // light gray bg

	printf("OpenGL version %s\n", glGetString(GL_VERSION));

	const char *vertex_shader_src = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;"
	"uniform mat4 transform;"
	"uniform mat4 view;"
	"uniform mat4 projection;"
    "void main()"
    "{"
    "   gl_Position = projection * view * transform * vec4(aPos, 1.0f);"
    "}\0";

	const char *fragment_shader_src = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main() {\n"
	"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}";

	glm::mat4 model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	glm::mat4 view          = glm::mat4(1.0f);
	glm::mat4 projection    = glm::mat4(1.0f);
	//model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	//projection = glm::perspective(glm::radians(102.0f), (float)1680 / (float)1050, 0.1f, 10000.0f);
	projection = glm::perspectiveLH(glm::radians(90.0f), (float)1680 / (float)1050, 0.1f, 10000.0f);
	//projection = glm::ortho(0.0f, 1680.0f, 0.0f, 1050.0f, 0.1f, 1000.0f);


	std::shared_ptr<Shader> my_shader(new Shader);
	my_shader->load_from_string(vertex_shader_src, fragment_shader_src);
	my_shader->set_active();
	my_shader->set_mat4("transform", model);
	my_shader->set_mat4("view", view);
	my_shader->set_mat4("projection", projection);

	std::vector<float> plane = {
		0.5f,  0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
		};
	std::shared_ptr<Object> test_obj(new Object(my_shader, plane));
	std::shared_ptr<Object> test_obj2(new Object(my_shader, plane));
	objects.push_back(test_obj);
	objects.push_back(test_obj2);
	test_obj->translate(glm::vec3(0.0f,0.0f,0.0f));
	test_obj2->translate(glm::vec3(-57.0f,22.0f,251.0f));
	test_obj2->scale({20,20,1});
	test_obj->scale({20,20,1});

	socket_fd = create_socket();

	bool running = true;
	printf("Starting main loop\n");
	uint64_t last_call = 0;
	while(running) {
		uint64_t delta = SDL_GetTicks() - last_call;
		update_gw2();

		glm::vec3 cameraPos   = glm::make_vec3(gw2_data.fCameraPosition);
		glm::vec3 cameraFront = glm::make_vec3(gw2_data.fCameraFront);
		glm::vec3 cameraUp    = glm::make_vec3(gw2_data.fCameraTop);
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		view = glm::lookAtLH(cameraPos, cameraPos + cameraFront, cameraUp);
		my_shader->set_mat4("view", view);
		update_gl(delta);
		SDL_GL_SwapWindow(window);
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
		
			switch(event.type) {
				case SDL_QUIT:
					running = false;
					break;
			}
		}
		glXSwapBuffers(mXDisplay, mXWindow);
		//SDL_RenderPresent(renderer);
		SDL_Delay(1000 / 60);
	}

	// Once finished with OpenGL functions, the SDL_GLContext can be deleted.
	//SDL_GL_DeleteContext(glcontext); 
	return 0;
} 
