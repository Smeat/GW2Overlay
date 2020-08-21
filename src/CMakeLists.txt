project(GW2Overlay)
cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(X11 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)
INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 COMPONENTS Widgets REQUIRED)


INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})

SET(GL_SOURCES
	overlay/renderer/gl/GLShader.cpp
	overlay/renderer/gl/GLTexture.cpp
	overlay/renderer/gl/GLMesh.cpp

	overlay/renderer/GLRenderer.cpp
	)


add_executable(GW2Overlay
	overlay/SDLMain.cpp
	overlay/Object.cpp
	utils/xml/pugixml.cpp
	overlay/Mesh.cpp
	utils/GW2Link.cpp
	utils/CategoryManager.cpp
	utils/POI.cpp
	overlay/QtMain.cpp
	overlay/qt/OptionsIcon.cpp
	overlay/qt/OptionsWindow.ui
	overlay/qt/OptionsWindow.cpp

	${GL_SOURCES}
	)
TARGET_LINK_LIBRARIES(GW2Overlay 
	${SDL2_LIBRARIES}
	${SDL2IMAGE_LIBRARIES}
	${X11_LIBRARIES}
	${OPENGL_LIBRARIES}
	${GLEW_LIBRARIES}
	xcb X11-xcb xcb-xfixes
	${Boost_LIBRARIES}
	${Qt5Widgets_LIBRARIES}
	Threads::Threads
	vulkan
	)


add_executable(vkTest
	overlay/renderer/main.cpp)

TARGET_LINK_LIBRARIES(vkTest
	${SDL2_LIBRARIES}
	${SDL2IMAGE_LIBRARIES}
	${X11_LIBRARIES}
	${OPENGL_LIBRARIES}
	${GLEW_LIBRARIES}
	xcb X11-xcb xcb-xfixes
	${Boost_LIBRARIES}
	${Qt5Widgets_LIBRARIES}
	Threads::Threads
	vulkan
	  )
