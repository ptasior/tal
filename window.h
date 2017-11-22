#pragma once
// SDL and  string included in net.h
#include "net.h"
#include "gl_header.h"

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
	std::string getGlLog(GLuint object);

	SDL_Window* mWindow = nullptr;
	SDL_Renderer *mRenderer = nullptr;
	SDL_GLContext mGLContext = nullptr;

	SDL_Texture *tex = nullptr;
	bool mQuit = false;

	Net mNet;

	GLuint vao;

	//Screen dimension constants
	int SCREEN_WIDTH = 640;
	int SCREEN_HEIGHT = 480;
};

