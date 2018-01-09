cmake_minimum_required(VERSION 2.8)


project(fontGen)

add_definitions(
		-DGLM_FORCE_RADIANS=1
	)

set(SOURCE_FILES
		main.cpp
	)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -pedantic -ftemplate-depth=2000")
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# This has to be clled after add_executable as it requires defined target
include(FindPkgConfig)

pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
pkg_search_module(SDL2TTF REQUIRED SDL2_ttf>=2.0.0)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}
						${SDL2_LIBRARIES}
						${SDL2NET_LIBRARIES}
						${SDL2IMAGE_LIBRARIES}
						${SDL2TTF_LIBRARIES}
						dl
					)

