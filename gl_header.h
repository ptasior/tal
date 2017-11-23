#ifdef __EMSCRIPTEN__
	#define GL_GLEXT_PROTOTYPES 1
	#include <SDL.h>
	#include <SDL_opengles2.h>
#else
	#include <GL/glew.h>

	#include <SDL2/SDL.h>
	#define GL_GLEXT_PROTOTYPES 1
	#include <SDL2/SDL_opengl.h>
#endif

