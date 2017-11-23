#include "window.h"
#include "log.h"
#include "net.h"
#include "shader.h"
#include "camera.h"
#include "scene.h"
#include "gui.h"

#ifdef __EMSCRIPTEN__
	#include <SDL_ttf.h>
#else
	#include <SDL2/SDL_ttf.h>
#endif

Window::Window()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetSwapInterval(0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1); // Transparency

#ifndef __EMSCRIPTEN__
	glewExperimental = true; // Needed in core profile
#endif

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		Log(Log::DIE) << "SDL could not initialize! SDL_Error: " << SDL_GetError();

	//Create window
	mWindow = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
							mScreenWidth, mScreenHeight,
							SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if(!mWindow)
		Log(Log::DIE) << "Window could not be created! SDL_Error: " << SDL_GetError();

	mGLContext = SDL_GL_CreateContext(mWindow);
	if(!mGLContext)
		Log(Log::DIE) << "GL context could not be created! SDL_Error: " << SDL_GetError();

	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if(!mRenderer)
		Log(Log::DIE) << "Renderer could not be created! SDL_Error: " << SDL_GetError();

	int fontsOK = TTF_Init();
	if(fontsOK == -1)
		Log(Log::DIE) << "Cannot initialise fonts " << TTF_GetError() << SDL_GetError();

#ifdef __EMSCRIPTEN__
	glGenVertexArraysOES(1, &vao);
	glBindVertexArrayOES(vao);
#else
	GLenum glew = glewInit();
	if(glew!= GLEW_OK)
		Log(Log::DIE) << "Failed to initialize GLEW";

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
#endif

	mNet = std::make_shared<Net>();

	mCamera = std::make_shared<Camera>();
	mCamera->init();
	mCamera->setSceneSize(mScreenWidth, mScreenHeight);

	mScene = std::make_shared<Scene>();
	mScene->init();
	mScene->setCamera(mCamera);

	mGui = std::make_shared<Gui>();
	mGui->init();
	mGui->setSceneSize(mScreenWidth, mScreenHeight);

	Log() << "Initialisation succesed";
}

Window::~Window()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	TTF_Quit();
	SDL_Quit();

	Log() << "Quit";
}

void Window::onEvent(SDL_Event &event)
{
	/* an event was found */
	switch (event.type)
	{
		case SDL_QUIT:
			mQuit = true;
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
			case SDLK_q:
					mQuit = true;
					break;
			case SDLK_p:
					Log() << "a log";
					break;
			case SDLK_n:
					Log() << "net";
					mNet->connect();
					mNet->send("new message");
					break;
			}
			break;
		case SDL_WINDOWEVENT:
					if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
						onResize(event.window.data1, event.window.data2);
					break;
	}
}

void Window::loop()
{
	while (!mQuit)
		onLoop();
}

void Window::onLoop()
{
	static SDL_Event event; // This SHOULD be safe

	if(SDL_PollEvent(&event))
		onEvent(event);

	mNet->loop();
	onPaint();
}

void Window::onPaint()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Shader::getShader("triangle")->use();
	mScene->paint();

	Shader::getShader("gui")->use();
	glDisable(GL_DEPTH_TEST);
	mGui->paint();

	SDL_GL_SwapWindow(mWindow);
}

void Window::onResize(int width, int height)
{
	mScreenWidth = width;
	mScreenHeight = height;
	glViewport(0, 0, mScreenWidth, mScreenHeight);

	mCamera->setSceneSize(mScreenWidth, mScreenHeight);
	mGui->setSceneSize(mScreenWidth, mScreenHeight);
}

