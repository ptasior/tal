cmake_minimum_required(VERSION 2.8)
project(tal)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES
		main.cpp
		log.cpp
		net.cpp
	)

# Rough but works for now
add_custom_target(js 
		COMMAND emcc -std=c++11
					-s USE_SDL=2
					-s USE_SDL_NET=2
					-s USE_SDL_IMAGE=2
					-s SDL2_IMAGE_FORMATS='["bmp"]'
					${SOURCE_FILES}
					-o "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.html"
					--preload-file img
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	)


add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# This has to be after add_executable as it requires defined target
include(FindPkgConfig)

pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2NET REQUIRED SDL2_net>=2.0.0)
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}
						${SDL2_LIBRARIES}
						${SDL2NET_LIBRARIES}
						${SDL2IMAGE_LIBRARIES}
					)

