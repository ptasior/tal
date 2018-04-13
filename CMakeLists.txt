cmake_minimum_required(VERSION 2.8)

option(JS "JS complation" OFF)
if("${JS}")
	# Yeah, I know, it's not portable... but how convenient!
	set(CMAKE_TOOLCHAIN_FILE "/home/ja/Tmp/cpp/emsdk-portable/emscripten/1.37.36/cmake/Modules/Platform/Emscripten.cmake")
endif()

project(tal)

add_definitions(
		-DGLM_FORCE_RADIANS=1
	)

set(SOURCE_FILES
		main.cpp
		log.cpp
		window.cpp
		net.cpp
		shader.cpp
		sprite.cpp
		sprite_gui.cpp
		texture.cpp
		camera.cpp
		camera_rotating.cpp
		camera_fps.cpp
		scene.cpp
		gui.cpp
		widget.cpp
		lua.cpp
		lua_scene.cpp
		lua_gui.cpp
		model.cpp
		model_obj.cpp
		map.cpp
		skybox.cpp
		matrix.cpp
		time.cpp
		config.cpp
		shared_data.cpp
	)


# add_custom_target(game_symlinks ALL
# 		COMMAND ln -sf ${PROJECT_SOURCE_DIR}/talisman game
#		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
# 	)

add_custom_target(game_symlinks ALL
		COMMAND ln -sf ${PROJECT_SOURCE_DIR}/letters game
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	)

add_custom_target(symlinks ALL
		COMMAND ln -sf ${PROJECT_SOURCE_DIR}/shaders .
		COMMAND ln -sf ${PROJECT_SOURCE_DIR}/lua_lib .
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	)

# -----------------------------------------------------------------------------
if("${JS}")
	add_custom_target(html_symlinks ALL
			COMMAND cp ${PROJECT_SOURCE_DIR}/tools/game.html .
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)

	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGLM_FORCE_RADIANS=1")
	# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ASSERTIONS=2")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL_NET=2")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL_IMAGE=2")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL_TTF=2")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s SDL2_IMAGE_FORMATS='[\"png\"]'")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_PTHREADS=1")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s FULL_ES2=1")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s TOTAL_MEMORY=335544320")
	# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ALLOW_MEMORY_GROWTH=1")

	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O2")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_PTHREADS=1")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -ldl")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file game")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file lua_lib")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file shaders")

	include_directories("lib")
	include_directories("lib/lua")
	include_directories("lib/sol")

	link_directories("${PROJECT_SOURCE_DIR}/lib/lua/")
	link_libraries("lua_emscripten")

	add_executable(${PROJECT_NAME} ${SOURCE_FILES})
	set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")

# -----------------------------------------------------------------------------
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -pedantic -ftemplate-depth=2000")
	add_executable(${PROJECT_NAME} ${SOURCE_FILES})

	# This has to be clled after add_executable as it requires defined target
	include(FindPkgConfig)

	pkg_search_module(SDL2 REQUIRED sdl2)
	pkg_search_module(SDL2NET REQUIRED SDL2_net>=2.0.0)
	pkg_search_module(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
	pkg_search_module(SDL2TTF REQUIRED SDL2_ttf>=2.0.0)

	include_directories(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})

	find_package(OpenGL)
	find_package(GLEW)
	include_directories(${GLEW_INCLUDE_DIRS})

	include_directories(lib/sol)
	include_directories(lib/lua)

	target_link_libraries(${PROJECT_NAME}
							${SDL2_LIBRARIES}
							${SDL2NET_LIBRARIES}
							${SDL2IMAGE_LIBRARIES}
							${SDL2TTF_LIBRARIES}
							dl
							${PROJECT_SOURCE_DIR}/lib/lua/liblua.a
							${OPENGL_gl_LIBRARY}
							${GLEW_LIBRARIES}
						)
endif()
# -----------------------------------------------------------------------------

