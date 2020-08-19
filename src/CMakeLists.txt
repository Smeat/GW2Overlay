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

add_executable(GW2Overlay
	ui/SDLMain.cpp
	ui/Shader.cpp
	ui/Object.cpp
	utils/xml/pugixml.cpp
	ui/Texture.cpp
	ui/Mesh.cpp
	utils/GW2Link.cpp
	utils/TacoLoader.cpp
	ui/QtMain.cpp
	ui/qt/OptionsIcon.cpp
	ui/qt/OptionsWindow.ui
	ui/qt/OptionsWindow.cpp
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
	)

#add_executable(QtGW2Overlay
	#ui/QtMain.cpp
	#ui/qt/OptionsIcon.cpp
	#ui/qt/OptionsWindow.ui
	#ui/qt/OptionsWindow.cpp
	#	)
#TARGET_LINK_LIBRARIES(QtGW2Overlay ${Qt5Widgets_LIBRARIES})
