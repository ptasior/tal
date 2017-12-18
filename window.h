#pragma once
// SDL and  string included in net.h
#include "gl_header.h"
#include <memory>

class Scene;
class Camera;
class Net;
class Gui;

class Window
{
public:
	Window();
	virtual ~Window();

	void processEvents();
	bool onEvent(SDL_Event &event);
	void onLoop();
	void onPaint();
	bool onClick(int x, int y);
	bool onDrag(int x, int y);
	bool onDrop(int x, int y);
	void onResize(int width, int height);

	void loop();

private:
	int mScreenWidth = 640;
	int mScreenHeight = 480;
	bool mQuit = false;
	bool mDragging = false;

	SDL_Window* mWindow = nullptr;
	SDL_Renderer *mRenderer = nullptr;
	SDL_GLContext mGLContext = nullptr;
	GLuint vao;

	std::shared_ptr<Net> mNet;
	std::shared_ptr<Camera> mCamera;
	std::shared_ptr<Scene> mScene;
	std::shared_ptr<Gui> mGui;
};

