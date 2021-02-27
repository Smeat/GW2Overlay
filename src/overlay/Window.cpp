#include "Window.h"

#include <X11/X.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/extensions/XI2.h>
#include <X11/extensions/XInput2.h>

#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

std::vector<std::pair<std::string, Window>> get_all_windows(Display* display) {
	Window root_window = XDefaultRootWindow(display);
	std::vector<Window> windows_to_process;
	windows_to_process.push_back(root_window);
	std::vector<std::pair<std::string, Window>> all_windows;

	while (!windows_to_process.empty()) {
		Window current_window = windows_to_process.back();
		windows_to_process.pop_back();
		XTextProperty text;
		char* name = 0;
		XFetchName(display, current_window, &name);
		std::string name_str;
		if (name) {
			name_str = name;
			XFree(name);
		}
		all_windows.push_back({name_str, current_window});

		Window root, parent;
		Window* children;
		unsigned int n;
		XQueryTree(display, current_window, &root, &parent, &children, &n);
		if (children != NULL) {
			for (int i = 0; i < n; ++i) {
				windows_to_process.push_back(children[i]);
			}
			XFree(children);
		}
	}
	return all_windows;
}

Window find_window(Display* display, const std::string& name) {
	auto windows = get_all_windows(display);
	for (auto iter = windows.begin(); iter != windows.end(); ++iter) {
		std::cout << iter->first << std::endl;
		if (iter->first == name) {
			return iter->second;
		}
	}
	return 0;
}

// TODO: only create the window once gw2 runs. recreate if window moves
WindowData createTransparentWindow(const char* title, int x, int y, int w, int h, bool create_gl_surface,
								   bool detect_gw2) {
	Window mXWindow;
	Display* mXDisplay;
	mXDisplay = XOpenDisplay(0);

	if (mXDisplay == 0) {
		printf("Failed to connect to the Xserver\n");
		return {0, 0};
	}
	Window gw2_window = find_window(mXDisplay, "Guild Wars 2");

	if (gw2_window && detect_gw2) {
		int gw2_x, gw2_y;
		Window root_win;
		unsigned int gw2_width, gw2_height;
		unsigned int gw2_border, gw2_depth;
		XGetGeometry(mXDisplay, gw2_window, &root_win, &gw2_x, &gw2_y, &gw2_width, &gw2_height, &gw2_border,
					 &gw2_depth);
		std::cout << "GW2 dimensions: " << gw2_x << ", " << gw2_y << " " << gw2_width << " " << gw2_height << " "
				  << gw2_border << " " << gw2_depth << std::endl;
		XWindowAttributes attrs;
		int abs_x, abs_y;
		Window child;
		XTranslateCoordinates(mXDisplay, gw2_window, root_win, 0, 0, &abs_x, &abs_y, &child);
		XGetWindowAttributes(mXDisplay, gw2_window, &attrs);
		std::cout << "Offset: " << abs_x - attrs.x << " " << abs_y - attrs.y << std::endl;
		x = abs_x - attrs.x;
		y = abs_y - attrs.y;
		w = gw2_width;
		h = gw2_height;
	}

	xcb_connection_t* xconn = XGetXCBConnection(mXDisplay);

	XVisualInfo visualinfo;
	XMatchVisualInfo(mXDisplay, DefaultScreen(mXDisplay), 32, TrueColor, &visualinfo);

	GC gc;
	XSetWindowAttributes attr;
	attr.colormap = XCreateColormap(mXDisplay, DefaultRootWindow(mXDisplay), visualinfo.visual, AllocNone);
	attr.event_mask = NoEventMask;
	attr.background_pixmap = None;
	attr.border_pixel = 0;

	mXWindow = XCreateWindow(mXDisplay, DefaultRootWindow(mXDisplay), x, y, w,
							 h,	 // x,y,width,height : are possibly opverwriteen by window manager
							 0, visualinfo.depth, InputOutput, visualinfo.visual,
							 CWColormap | CWEventMask | CWBackPixmap | CWBorderPixel, &attr);
	gc = XCreateGC(mXDisplay, mXWindow, 0, 0);
	printf("Window has id: %lu\n", mXWindow);

	int baseEventMask = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_PROPERTY_CHANGE |
						XCB_EVENT_MASK_FOCUS_CHANGE;
	int transparentForInputEventMask = baseEventMask | XCB_EVENT_MASK_VISIBILITY_CHANGE |
									   XCB_EVENT_MASK_RESIZE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
									   XCB_EVENT_MASK_COLOR_MAP_CHANGE | XCB_EVENT_MASK_OWNER_GRAB_BUTTON;
	const int mask = XCB_CW_OVERRIDE_REDIRECT | XCB_CW_EVENT_MASK;
	const int values[] = {1, transparentForInputEventMask};
	xcb_void_cookie_t cookie = xcb_change_window_attributes_checked(xconn, mXWindow, mask, values);
	xcb_generic_error_t* error;
	if ((error = xcb_request_check(xconn, cookie))) {
		fprintf(stderr, "Could not reparent the window\n");
		free(error);
		return {0, 0};
	} else {
		printf("Changed attributes\n");
	}
	// Mouse passthrough
	// init xfixes
	const xcb_query_extension_reply_t* reply = xcb_get_extension_data(xconn, &xcb_xfixes_id);
	if (!reply || !reply->present) {
		return {0, 0};
	}

	auto xfixes_query = xcb_xfixes_query_version(xconn, XCB_XFIXES_MAJOR_VERSION, XCB_XFIXES_MINOR_VERSION);
	auto xfixesQuery = xcb_xfixes_query_version_reply(xconn, xfixes_query, NULL);
	if (!xfixesQuery || xfixesQuery->major_version < 2) {
		printf("failed to initialize XFixes\n");
		return {0, 0};
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
	xcb_xfixes_set_window_shape_region_checked(xconn, mXWindow, XCB_SHAPE_SK_INPUT, 0, 0, region);
	xcb_xfixes_destroy_region(xconn, region);

	if (create_gl_surface) {
		GLXContext glcontext = glXCreateContext(mXDisplay, &visualinfo, 0, True);
		if (!glcontext) {
			printf("Failed to create openGL context\n");
			return {0, 0};
		}
		glXMakeCurrent(mXDisplay, mXWindow, glcontext);
	}

	XMapWindow(mXDisplay, mXWindow);
	return {mXDisplay, mXWindow};
}

void setup_input_events(WindowData window, std::function<void(std::string)> cb, const bool* running) {
	window.display = XOpenDisplay(":0");
	int xiOpcode, queryEvent, queryError;
	if (!XQueryExtension(window.display, "XInputExtension", &xiOpcode, &queryEvent, &queryError)) {
		printf("Failed to find XInput!\n");
		return;
	}

	XIEventMask eventMask;
	eventMask.deviceid = XIAllMasterDevices;
	eventMask.mask_len = XIMaskLen(XI_LASTEVENT);
	eventMask.mask = (unsigned char*)calloc(eventMask.mask_len, sizeof(char));
	XISetMask(eventMask.mask, XI_RawKeyPress);
	XISelectEvents(window.display, DefaultRootWindow(window.display), &eventMask, 1);
	XSync(window.display, false);
	free(eventMask.mask);

	while (*running) {
		XEvent event;
		XGenericEventCookie* cookie = (XGenericEventCookie*)&event.xcookie;
		XNextEvent(window.display, &event);

		if (XGetEventData(window.display, cookie)) {
			if (cookie->evtype == XI_RawKeyPress) {
				XIRawEvent* raw_event = (XIRawEvent*)cookie->data;
				KeySym sym = XkbKeycodeToKeysym(window.display, raw_event->detail, 0, 0);
				std::string str = XKeysymToString(sym);
				cb(str);
			}
		}
	}
}
