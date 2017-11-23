#pragma once
// SDL and  string included in net.h
#include "gl_header.h"
#include <memory>

class Scene;
class Net;
class Gui;

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
	int mScreenWidth = 640;
	int mScreenHeight = 480;
	bool mQuit = false;

	SDL_Window* mWindow = nullptr;
	SDL_Renderer *mRenderer = nullptr;
	SDL_GLContext mGLContext = nullptr;
	GLuint vao;

	std::shared_ptr<Net> mNet;
	std::shared_ptr<Scene> mScene;
	std::shared_ptr<Gui> mGui;
};

