#pragma once
// SDL and  string included in net.h
#include "gl_header.h"
#include <memory>

class Renderer;

class Window
{
public:
	Window();
	~Window();

	void init();
	void initObjects();
	bool loop();
	void updateSize();

private:
	void processEvents();
	bool onEvent(SDL_Event &event);
	bool onClick(int x, int y);
	bool onDrag(int x, int y);
	bool onDrop(int x, int y);
	void onResize(int width, int height);

	void onPaint();

private:
	int mScreenWidth = 640;
	int mScreenHeight = 480;
	bool mQuit = false;
	bool mDragging = false;
	bool mButtonDown = false;

	Renderer *mRenderer = nullptr;

	SDL_Window* mWindow = nullptr;
	SDL_Renderer *mSDLRenderer = nullptr;
	SDL_GLContext mGLContext = nullptr;
	GLuint vao;
};

