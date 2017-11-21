#pragma once
// SDL and  string included in net.h
#include "net.h"
#ifdef __EMSCRIPTEN__
	#define GL_GLEXT_PROTOTYPES 1
	#include <SDL_opengles2.h>
#else
	#include <GL/glew.h>

	#include <SDL2/SDL.h>
	#define GL_GLEXT_PROTOTYPES 1
	#include <SDL2/SDL_opengl.h>
#endif

class Window
{
public:
	Window();
	~Window();

	void onEvent(SDL_Event &event);
	void onLoop();
	void onPaint();
	void onResize(int width, int height);

	void loop();


private:
	GLuint loadShader(const char * file, GLenum type);
	std::string readFile(const char* filename);
	std::string getGlLog(GLuint object);

	SDL_Window* mWindow = nullptr;
	SDL_Renderer *mRenderer = nullptr;
	SDL_GLContext mGLContext = nullptr;

	SDL_Texture *tex = nullptr;
	bool mQuit = false;

	Net mNet;

	GLuint vao;
	GLuint shaderProgram;

	//Screen dimension constants
	int SCREEN_WIDTH = 640;
	int SCREEN_HEIGHT = 480;
};

