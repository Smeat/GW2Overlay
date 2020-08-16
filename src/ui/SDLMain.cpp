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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <X11/X.h>
#include <X11/Xutil.h>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <thread>
#include <iostream>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xfixes.h>
#include <xcb/shape.h>
#include <xcb/xinput.h>

#include "Shader.h"

Window        mXWindow;
Display*      mXDisplay;

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

	rectangle.x = 1280;
	rectangle.y = 0;
	rectangle.width = w+ 1280;
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

void add_object(float x, float y, float z) {
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
	trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
	unsigned int transformLoc = glGetUniformLocation(1, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
	glLoadIdentity();
	glPushMatrix();
	glTranslatef(x, y, z);
	glBegin(GL_POLYGON);
	// TODO: draw object
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.5, 0.0, 0.0);
	glVertex3f(0.5, 0.5, 0.0);
	glVertex3f(0.0, 0.5, 0.0);
	glEnd();
	glPopMatrix();
}

void update_gl(int delta){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBegin(GL_POLYGON);
	static int val = 0;
	val += 1;
	val = val % 50;
	/*glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.5, 0.0, 0.0);
	glVertex3f(0.5, 0.5, 0.0);
	glVertex3f(0.0, 0.5, 0.0);*/
	glVertex3f(0.5f,  0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glVertex3f(-0.5f,  0.5f, 0.0f);
	glEnd();
	//add_object(-0.5,-0.5,0);

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
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	projection = glm::perspective(glm::radians(45.0f), (float)1680 / (float)1050, 0.1f, 100.0f);


	Shader my_shader;
	my_shader.load_from_string(vertex_shader_src, fragment_shader_src);
	my_shader.set_active();
	my_shader.set_mat4("transform", model);
	my_shader.set_mat4("view", view);
	my_shader.set_mat4("projection", projection);
	glClear(GL_COLOR_BUFFER_BIT);

	bool running = true;
	printf("Starting main loop\n");
	uint64_t last_call = 0;
	while(running) {
		uint64_t delta = SDL_GetTicks() - last_call;
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
