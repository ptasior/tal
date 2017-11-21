cmake_minimum_required(VERSION 2.8)
project(tal)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES
		main.cpp
		log.cpp
		window.cpp
		net.cpp
	)

# Rough but works for now
add_custom_target(js 
		COMMAND emcc -std=c++11
					-s USE_SDL=2
					-s USE_SDL_NET=2
					-s USE_SDL_IMAGE=2
					-s SDL2_IMAGE_FORMATS='["bmp"]'
					-s USE_PTHREADS=1
					-s FULL_ES2=1
					#-s USE_GLFW=3
					-I lib/selene
					-I lib/lua
					-s TOTAL_MEMORY=167772160
					${SOURCE_FILES}
					${PROJECT_SOURCE_DIR}/lib/lua/liblua_emscripten.a
					-o "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.html"
					-s USE_PTHREADS=1
					-ldl
					--preload-file assets
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	)


add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# This has to be after add_executable as it requires defined target
include(FindPkgConfig)

pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2NET REQUIRED SDL2_net>=2.0.0)
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})

find_package(OpenGL)
#find_package(GLUT)
find_package(GLEW)
include_directories(${GLEW_INCLUDE_DIRS})

include_directories(lib/selene)
include_directories(lib/lua)

target_link_libraries(${PROJECT_NAME}
						${SDL2_LIBRARIES}
						${SDL2NET_LIBRARIES}
						${SDL2IMAGE_LIBRARIES}
						dl
						${PROJECT_SOURCE_DIR}/lib/lua/liblua.a
						${OPENGL_gl_LIBRARY}
						#${GLUT_LIBRARIES}
						${GLEW_LIBRARIES}
					)

