#include "window.h"
#include "log.h"
#include "net.h"
#include "shader.h"
#include "rotating_camera.h"
#include "fps_camera.h"
#include "scene.h"
#include "gui.h"
#include "lua.h"
#include "time.h"

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
		Log(Log::DIE) << "Window: SDL could not initialize! SDL_Error: " << SDL_GetError();

	//Create window
	mWindow = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
							mScreenWidth, mScreenHeight,
							SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if(!mWindow)
		Log(Log::DIE) << "Window: Window could not be created! SDL_Error: " << SDL_GetError();

	mGLContext = SDL_GL_CreateContext(mWindow);
	if(!mGLContext)
		Log(Log::DIE) << "Window: GL context could not be created! SDL_Error: " << SDL_GetError();

	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if(!mRenderer)
		Log(Log::DIE) << "Window: Renderer could not be created! SDL_Error: " << SDL_GetError();

	int fontsOK = TTF_Init();
	if(fontsOK == -1)
		Log(Log::DIE) << "Window: Cannot initialise fonts " << TTF_GetError() << SDL_GetError();

#ifdef __EMSCRIPTEN__
	glGenVertexArraysOES(1, &vao);
	glBindVertexArrayOES(vao);
#else
	GLenum glew = glewInit();
	if(glew!= GLEW_OK)
		Log(Log::DIE) << "Window: Failed to initialize GLEW";

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
#endif

	mNet = std::make_shared<Net>();

	// mCamera = std::make_shared<RotatingCamera>();
	mCamera = std::make_shared<FpsCamera>();
	mCamera->init();
	mCamera->setSceneSize(mScreenWidth, mScreenHeight);

	mScene = std::make_shared<Scene>();
	mScene->init();
	mScene->setCamera(mCamera);

	mGui = std::make_shared<Gui>();
	mGui->init();
	mGui->setSceneSize(mScreenWidth, mScreenHeight);


	Lua::getInstance()->initGui(mGui.get());
	Lua::getInstance()->initCamera(mCamera.get());
	Lua::getInstance()->initScene(mScene.get());
	Lua::getInstance()->run();

	Log() << "Window: Initialisation succesed";
}

Window::~Window()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	TTF_Quit();
	SDL_Quit();

	Log() << "Window: Quit";
}

void Window::processEvents()
{
	// SDL_PumpEvents();
	const Uint8 *state = SDL_GetKeyboardState(NULL);

	if(mCamera->processEvents(state)) return;
}

bool Window::onEvent(SDL_Event &event)
{
	switch (event.type)
	{
		case SDL_QUIT:
			mQuit = true;
			return true;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
				case SDLK_ESCAPE:
					mQuit = true;
					break;
				/*case SDLK_n:
					Log() << "net";
					mNet->connect();
					mNet->send("new message");
					break;
				case SDLK_f:
					{
						static bool wf = false;
						wf = !wf;
					#ifndef __EMSCRIPTEN__
						glPolygonMode(GL_FRONT_AND_BACK, wf?GL_LINE:GL_FILL);
					#endif
					}
					break;*/
				case SDLK_BACKSPACE:
					if(mGui->textInput("backspace"))
						return true;
					break;
				case SDLK_RETURN:
					if(mGui->textInput("return"))
						return true;
					break;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			onClick(event.button.x, event.button.y);
			mDragging = true;
			return true;
		case SDL_MOUSEMOTION:
			if(mDragging)
				onDrag(event.button.x, event.button.y);
			return mDragging;
		case SDL_MOUSEBUTTONUP:
			if(mDragging)
				onDrop(event.button.x, event.button.y);
			mDragging = false;
			return true;
		case SDL_TEXTINPUT:
			if(mGui->textInput(event.text.text))
				return true;
			break;
		case SDL_WINDOWEVENT:
			if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				onResize(event.window.data1, event.window.data2);
			return true;
	}

	return false;
}

void Window::loop()
{
	while (!mQuit)
		onLoop();
}

void Window::onLoop()
{
	static SDL_Event event; // This SHOULD be safe

	Time::registerNextFrame();

	if(SDL_PollEvent(&event))
		onEvent(event);

	if(!mGui->grabsFocus())
		processEvents();

	mNet->loop();
	onPaint();
}

void Window::onPaint()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// glEnable(GL_CULL_FACE);
	mScene->paint();

	// // glDisable(GL_CULL_FACE);
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

void Window::onClick(int x, int y)
{
	mGui->click(x, y);
}

void Window::onDrag(int x, int y)
{
	mGui->drag(x, y);
}

void Window::onDrop(int x, int y)
{
	mGui->drop(x, y);
}

