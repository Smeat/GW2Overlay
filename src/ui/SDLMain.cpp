#include <SDL2/SDL.h> 
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h> 
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h> 
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_syswm.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include <X11/X.h>
#include <chrono>
#include <thread>
#include <iostream>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

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

	// query Visual for "TrueColor" and 32 bits depth (RGBA)
	XVisualInfo visualinfo;
	XMatchVisualInfo(mXDisplay, DefaultScreen(mXDisplay), 32, TrueColor, &visualinfo);

	// create window
	GC gc;
	XSetWindowAttributes attr;
	attr.colormap   = XCreateColormap(mXDisplay, DefaultRootWindow(mXDisplay), visualinfo.visual, AllocNone);
	attr.event_mask = ExposureMask;// | KeyPressMask;
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

bool setWindowTransparency(Window handle, unsigned char alpha) {
	Display* display = XOpenDisplay(NULL);
	unsigned long opacity = (0xffffffff / 0xff) * alpha;
	Atom property = XInternAtom(display, "_NET_WM_WINDOW_OPACITY", false);
	if (property != None) {
		XChangeProperty(display, handle, property, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&opacity, 1);
		XFlush(display);
		XCloseDisplay(display);
		return true;
	}
	else {
		std::cerr << "Error" << std::endl;
		XCloseDisplay(display);
		return false;
	}
}

int main() {
	// retutns zero on success else non-zero 
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) { 
		printf("error initializing SDL: %s\n", SDL_GetError()); 
	} 
	SDL_Window* window = SDL_CreateTransparentWindow("GAME", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 1000);
	int w_flags = SDL_WINDOW_BORDERLESS | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_ALWAYS_ON_TOP;
//	SDL_Window* window = SDL_CreateWindow("GAME", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 1000, w_flags);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	//SDL_SetWindowOpacity(window, 0);
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 1);
	SDL_RenderClear(renderer);
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);  // Initialize wmInfo
	SDL_GetWindowWMInfo(window, &wmInfo);
	Window hWnd = wmInfo.info.x11.window;
//	setWindowTransparency(hWnd, 0);

	SDL_GLContext glcontext = SDL_GL_CreateContext(window);
	glClearColor( 0, 0.5, 0, 0.5) ; // light gray bg
	glClear(GL_COLOR_BUFFER_BIT);

	bool running = true;
	while(running) {
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POLYGON);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.5, 0.0, 0.0);
        glVertex3f(0.5, 0.5, 0.0);
        glVertex3f(0.0, 0.5, 0.0);
    glEnd();
    glFlush();
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
		
			switch(event.type) {
				case SDL_QUIT:
					running = false;
					break;
			}
		}
		SDL_RenderPresent(renderer);
		SDL_Delay(1000 / 60);
	}

	// Once finished with OpenGL functions, the SDL_GLContext can be deleted.
	SDL_GL_DeleteContext(glcontext); 
	return 0; 
} 
