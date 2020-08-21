#include "Window.h"

WindowData createTransparentWindow(const char* title, int x, int y, int w, int h, bool create_gl_surface) {
	Window mXWindow;
	Display* mXDisplay;
	mXDisplay = XOpenDisplay(0);

	if (mXDisplay == 0) {
		printf("Failed to connect to the Xserver\n");
		return {0, 0};
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
