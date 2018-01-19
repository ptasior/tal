#pragma once
// SDL and  string included in net.h
#include "gl_header.h"
#include <memory>

class Scene;
class Camera;
class Net;
class Gui;
class Config;

class Window
{
public:
	Window();
	~Window();

	void init();
	void processEvents();
	bool onEvent(SDL_Event &event);
	void onLoop();
	void onPaint();
	bool onClick(int x, int y);
	bool onDrag(int x, int y);
	bool onDrop(int x, int y);
	void onResize(int width, int height);

	Camera* getCamera();
	Scene* getScene();
	Gui* getGui();
	Config* getConfig();

	void loop();

private:
	int mScreenWidth = 640;
	int mScreenHeight = 480;
	bool mQuit = false;
	bool mDragging = false;
	bool mButtonDown = false;

	SDL_Window* mWindow = nullptr;
	SDL_Renderer *mRenderer = nullptr;
	SDL_GLContext mGLContext = nullptr;
	GLuint vao;

	std::shared_ptr<Net> mNet;
	std::shared_ptr<Camera> mCamera;
	std::shared_ptr<Scene> mScene;
	std::shared_ptr<Gui> mGui;
	std::shared_ptr<Config> mConfig;
};

extern Window *global_window;

