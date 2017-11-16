all:
	g++ --std=c++14 `sdl2-config --cflags` main.cpp log.cpp `sdl2-config --libs` -lSDL2 -lSDL2_net

js:
	. ~/Tmp/cpp/emsdk-portable/emsdk_set_env.sh
	emcc -std=c++11 -s USE_SDL=2 -s USE_SDL_NET=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["bmp"]' main.cpp log.cpp -o main.html --preload-file img
	# emcc -s USE_SDL=2 -s USE_SDL_NET=2 -s ASYNCIFY=1 cli.cpp -o cli.html

