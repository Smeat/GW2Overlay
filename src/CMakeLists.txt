project(GW2Overlay)
cmake_minimum_required(VERSION 3.8)

find_package(X11 REQUIRED)
find_package(OpenGL REQUIRED)
INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})

add_executable(GW2Overlay ui/SDLMain.cpp ui/Shader.cpp ui/Object.cpp utils/xml/pugixml.cpp ui/Texture.cpp ui/Mesh.cpp)
TARGET_LINK_LIBRARIES(GW2Overlay ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${X11_LIBRARIES} ${OPENGL_LIBRARIES} xcb X11-xcb xcb-xfixes glut GLEW)