#include "VKRenderer.h"
int main() {
	VKRenderer app;
	app.initWindow();
	app.initVulkan();

	try {
		app.mainLoop();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	app.cleanup();

	return EXIT_SUCCESS;
}
