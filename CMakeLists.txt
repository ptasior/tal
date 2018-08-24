cmake_minimum_required (VERSION 2.8)

option (JS "JS complation" OFF)
option (ANDROID "ANDROID complation" OFF)

if ("${JS}")
	# Yeah, I know, it's not portable... but how convenient!
	set (CMAKE_TOOLCHAIN_FILE "/home/ja/Tmp/cpp/emsdk-portable/emscripten/1.37.36/cmake/Modules/Platform/Emscripten.cmake")
endif ()

if (NOT "${ANDROID}")
	project (tal)
else()
	project (main)
endif ()

add_definitions (
		-DGLM_FORCE_RADIANS=1
	)

set (SOURCE_FILES
		${PROJECT_SOURCE_DIR}/src/main.cpp
		${PROJECT_SOURCE_DIR}/src/game.cpp
		${PROJECT_SOURCE_DIR}/src/window.cpp
		# ${PROJECT_SOURCE_DIR}/src/lua.cpp
		# ${PROJECT_SOURCE_DIR}/src/lua_scene.cpp
		# ${PROJECT_SOURCE_DIR}/src/lua_gui.cpp
		${PROJECT_SOURCE_DIR}/src/game_file.cpp
		${PROJECT_SOURCE_DIR}/src/file_utils.cpp
		${PROJECT_SOURCE_DIR}/src/stream_reader.cpp
		${PROJECT_SOURCE_DIR}/src/shader.cpp
		${PROJECT_SOURCE_DIR}/src/sprite.cpp
		${PROJECT_SOURCE_DIR}/src/sprite_gui.cpp
		${PROJECT_SOURCE_DIR}/src/texture.cpp
		${PROJECT_SOURCE_DIR}/src/camera.cpp
		${PROJECT_SOURCE_DIR}/src/camera_rotating.cpp
		${PROJECT_SOURCE_DIR}/src/camera_fps.cpp
		${PROJECT_SOURCE_DIR}/src/scene.cpp
		${PROJECT_SOURCE_DIR}/src/gui.cpp
		${PROJECT_SOURCE_DIR}/src/widget.cpp
		${PROJECT_SOURCE_DIR}/src/model.cpp
		${PROJECT_SOURCE_DIR}/src/model_obj.cpp
		${PROJECT_SOURCE_DIR}/src/map.cpp
		${PROJECT_SOURCE_DIR}/src/skybox.cpp
		${PROJECT_SOURCE_DIR}/src/log.cpp
		${PROJECT_SOURCE_DIR}/src/config.cpp
		${PROJECT_SOURCE_DIR}/src/string_utils.cpp
		${PROJECT_SOURCE_DIR}/src/matrix.cpp
		${PROJECT_SOURCE_DIR}/src/time.cpp
		# ${PROJECT_SOURCE_DIR}/src/net.cpp
		# ${PROJECT_SOURCE_DIR}/src/shared_data.cpp
		${PROJECT_SOURCE_DIR}/assets/output/embedded_defaults.cpp
	)

include_directories (
		${PROJECT_SOURCE_DIR}/assets/output/
	)


if (NOT "${ANDROID}")
	# add_custom_target (game_symlinks ALL
	# 		COMMAND ln -sf ${PROJECT_SOURCE_DIR}/talisman game
	# 		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	# 	)
    #
	# add_custom_target (game_symlinks ALL
	# 		COMMAND ln -sf ${PROJECT_SOURCE_DIR}/test game
	# 		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	# 	)
    #
	# add_custom_target (symlinks ALL
	# 		COMMAND ln -sf ${PROJECT_SOURCE_DIR}/shaders .
	# 		COMMAND ln -sf ${PROJECT_SOURCE_DIR}/lua_lib .
	# 		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	# 	)

	# add_custom_target (symlinks ALL
	# 		COMMAND ln -sf ${PROJECT_SOURCE_DIR}/output/letters.game data.game
	# 		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	# 	)
endif ()

# -----------------------------------------------------------------------------
if ("${JS}")
	add_definitions (
		-DJS=1
		)

	add_custom_target(html_symlinks ALL
			COMMAND cp ${PROJECT_SOURCE_DIR}/tools/game.html index.html
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)

	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGLM_FORCE_RADIANS=1")
	# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ASSERTIONS=2")
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2")
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL_NET=2")
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL_IMAGE=2")
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL_TTF=2")
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s SDL2_IMAGE_FORMATS='[\"png\"]'")
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_PTHREADS=1")
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s FULL_ES2=1")
	# SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s WASM=1")
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s TOTAL_MEMORY=335544320")
	# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ALLOW_MEMORY_GROWTH=1")

	SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O2")
	SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_PTHREADS=1")
	SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -ldl")
	# SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file game")
	# SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file lua_lib")
	# SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file shaders")
	SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file data.game")

	include_directories ("lib")
	include_directories ("lib/lua")
	include_directories ("lib/sol")

	link_directories ("${PROJECT_SOURCE_DIR}/lib/lua/")
	link_libraries ("lua_emscripten")

	add_executable (${PROJECT_NAME} ${SOURCE_FILES})
	set_target_properties (${PROJECT_NAME} PROPERTIES SUFFIX ".html")

# -----------------------------------------------------------------------------
elseif ("${ANDROID}")
	add_definitions (
			-DANDROID=1
			-D__ANDROID_API__=21
		)

	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -pedantic -ftemplate-depth=2000")

	string (TOLOWER "${CMAKE_BUILD_TYPE}" ANDROID_BUILD_DIR )
	set (DISTRIBUTION_DIR ${CMAKE_SOURCE_DIR}/android/distribution/android/SDL2/intermediates/ndkBuild/)

	add_library (SDL2 SHARED IMPORTED)
	add_library (SDL2_image SHARED IMPORTED)
	add_library (SDL2_net SHARED IMPORTED)
	add_library (SDL2_ttf SHARED IMPORTED)

	set_target_properties (SDL2 PROPERTIES IMPORTED_LOCATION
		${DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/local/${ANDROID_ABI}/libSDL2.so)
	set_target_properties (SDL2_image PROPERTIES IMPORTED_LOCATION
		${DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/local/${ANDROID_ABI}/libSDL2_image.so)
	set_target_properties (SDL2_net PROPERTIES IMPORTED_LOCATION
		${DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/local/${ANDROID_ABI}/libSDL2_net.so)
	set_target_properties (SDL2_ttf PROPERTIES IMPORTED_LOCATION
		${DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/local/${ANDROID_ABI}/libSDL2_ttf.so)


	add_library (${PROJECT_NAME} SHARED
			${PROJECT_SOURCE_DIR}/lib/SDL2/SDL2/src/main/android/SDL_android_main.c
			${SOURCE_FILES}
		)

	include_directories ("lib/SDL2/SDL2/include")
	include_directories ("lib/SDL2/SDL2_image")
	include_directories ("lib/SDL2/SDL2_net")
	include_directories ("lib/SDL2/SDL2_ttf")
	include_directories (lib/sol)
	include_directories (lib/lua)
	include_directories (lib)
	include_directories (lib/SDL2)
	include_directories ("android/ndk")

	target_link_libraries (${PROJECT_NAME}
			SDL2
			SDL2_image
			SDL2_net
			SDL2_ttf
			${CMAKE_SOURCE_DIR}/android/distribution/android/lua/intermediates/ndkBuild/release/obj/local/${ANDROID_ABI}/liblua.so
			GLESv2
			log
		)

# -----------------------------------------------------------------------------
else ()
	add_definitions (
		-DDESKTOP=1
		)

	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -pedantic -ftemplate-depth=2000")
	add_executable (${PROJECT_NAME} ${SOURCE_FILES})


	# This has to be clled after add_executable as it requires defined target
	include (FindPkgConfig)

	pkg_search_module (SDL2 REQUIRED sdl2)
	pkg_search_module (SDL2NET REQUIRED SDL2_net>=2.0.0)
	pkg_search_module (SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
	pkg_search_module (SDL2TTF REQUIRED SDL2_ttf>=2.0.0)

	include_directories (${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})

	find_package (OpenGL)
	find_package (GLEW)
	include_directories (${GLEW_INCLUDE_DIRS})

	include_directories (lib/lua)
	include_directories (lib/sol)

	target_link_libraries (${PROJECT_NAME}
							${SDL2_LIBRARIES}
							${SDL2NET_LIBRARIES}
							${SDL2IMAGE_LIBRARIES}
							${SDL2TTF_LIBRARIES}
							dl
							${PROJECT_SOURCE_DIR}/lib/lua/liblua.a
							${OPENGL_gl_LIBRARY}
							${GLEW_LIBRARIES}
						)
endif ()
# -----------------------------------------------------------------------------

