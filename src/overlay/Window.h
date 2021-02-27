#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <GL/glew.h>

#include <GL/gl.h>
#include <GL/glx.h>

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib-xcb.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <xcb/shape.h>
#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xfixes.h>
#include <xcb/xinput.h>
#include <xcb/xproto.h>

#include <cstdio>
#include <cstdlib>

#include <functional>
#include <string>

struct WindowData {
	Display* display;
	Window window;
};

WindowData createTransparentWindow(const char* title, int x, int y, int w, int h, bool create_gl_surface = false,
								   bool detect_gw2 = true);
void setup_input_events(WindowData window, std::function<void(std::string)> cb, const bool* running);

#endif
