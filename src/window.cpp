#include "window.h"
#include "log.h"
#include "camera_rotating.h"
#include "camera_fps.h"
#include "scene.h"
#include "gui.h"
#include "time.h"
#include "config.h"
#include "global.h"
#include "renderer.h"
#include <thread>
#include <chrono>

#ifdef DESKTOP
	#include <SDL2/SDL_ttf.h>
#else
	#include <SDL_ttf.h>
#endif

#if defined(DESKTOP) && ! defined(NDEBUG)
void GLAPIENTRY MessageCallback(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam
	)
{
	// if(severity >= DEBUG_SEVERITY_MEDIUM)
	if(type == GL_DEBUG_TYPE_ERROR)
		Log(Log::DIE) << "Window: GL error: "
				<< "type: " << type << " severity: " << severity
				<< Log::endl << "  " << message;
	else
		Log() << "Window: GL mesage: "
				<< "type: " << type << " severity: " << severity
				<< Log::endl << "  " << message;
}
#endif


Window::Window()
{}

void Window::init()
{
	Log() << "Window: Initialising";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetSwapInterval(0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1); // Transparency

#ifdef DESKTOP
	glewExperimental = true; // Needed in core profile
#endif

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		Log(Log::DIE) << "Window: SDL could not initialize! SDL_Error: " << SDL_GetError();

	//Create window
	mWindow = SDL_CreateWindow(Global::get<Config>()->get("windowName").c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
							mScreenWidth, mScreenHeight,
							SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if(!mWindow)
		Log(Log::DIE) << "Window: Window could not be created! SDL_Error: " << SDL_GetError();

	mGLContext = SDL_GL_CreateContext(mWindow);
	if(!mGLContext)
		Log(Log::DIE) << "Window: GL context could not be created! SDL_Error: " << SDL_GetError();

	mSDLRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if(!mSDLRenderer)
		Log(Log::DIE) << "Window: Renderer could not be created! SDL_Error: " << SDL_GetError();

	int fontsOK = TTF_Init();
	if(fontsOK == -1)
		Log(Log::DIE) << "Window: Cannot initialise fonts " << TTF_GetError() << SDL_GetError();

#ifndef DESKTOP
	glGenVertexArraysOES(1, &vao);
	glBindVertexArrayOES(vao);
#else
	GLenum glew = glewInit();
	if(glew!= GLEW_OK)
		Log(Log::DIE) << "Window: Failed to initialize GLEW";

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
#endif


#if defined(DESKTOP) && ! defined(NDEBUG)
	glEnable              ( GL_DEBUG_OUTPUT );
	glDebugMessageCallback( MessageCallback, 0 );
#endif

	Log() << "GL_VERSION: " << (char*)glGetString(GL_VERSION);
	Log() << "GL_RENDERER:" << (char*)glGetString(GL_RENDERER);
	Log() << "GL_SHADING_LANGUAGE_VERSION: " << (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

	mRenderer = Global::get<Renderer>();

	Log() << "Window: Initialisation succesed";
}

Window::~Window()
{
	SDL_DestroyRenderer(mSDLRenderer);
	SDL_DestroyWindow(mWindow);
	TTF_Quit();
	SDL_Quit();

	Log() << "Window: Quit";
}

void Window::updateSize()
{
	SDL_GetWindowSize(mWindow, &mScreenWidth, &mScreenHeight);
	onResize(mScreenWidth, mScreenHeight);
}

void Window::processEvents()
{
	// SDL_PumpEvents();
	const Uint8 *state = SDL_GetKeyboardState(NULL);

	if(mRenderer->getCamera())
		mRenderer->getCamera()->processEvents(state);
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
				// case SDLK_n:
				// 	Log() << "net";
				// 	mNet->connect();
				// 	mNet->send("new message");
				// 	break;
				// case SDLK_f:
				// 	{
				// 	static int cnt = 0;
				// 	global_sharedData->root().at("test")->set(std::to_string(cnt++));
				// 	}
					break;
				case SDLK_BACKSPACE:
					if(mRenderer->getGui() &&
							mRenderer->getGui()->textInput("backspace"))
						return true;
					break;
				case SDLK_RETURN:
					if(mRenderer->getGui() &&
							mRenderer->getGui()->textInput("return"))
						return true;
					break;
			}
			break;
		// case SDL_FINGERDOWN:
		// case SDL_FINGERMOTION:
		// case SDL_FINGERUP:
		case SDL_MOUSEBUTTONDOWN:
			if(onClick(event.button.x, event.button.y))
				return true;
			mButtonDown = true;
			return true;
		case SDL_MOUSEMOTION:
			if(mButtonDown)
			{
				mDragging = true;
				return onDrag(event.button.x, event.button.y);
			}
			return false;
		case SDL_MOUSEBUTTONUP:
			mButtonDown = false;
			if(mDragging)
			{
				bool ret = onDrop(event.button.x, event.button.y);
				mDragging = false;
				return ret;
			}
			return false;
		case SDL_TEXTINPUT:
			if(mRenderer->getGui() &&
					mRenderer->getGui()->textInput(event.text.text))
				return true;
			break;
		case SDL_WINDOWEVENT:
			if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				onResize(event.window.data1, event.window.data2);
			return true;
	}

	return false;
}

bool Window::loop()
{
	SDL_Event mEvent;

	Global::get<Time>()->registerNextFrame();

	if(SDL_PollEvent(&mEvent))
	{
		bool processed = false;

		processed = onEvent(mEvent);
		
		if(!processed) // If event was handled in Gui, do not pass it to the camera
			if(mRenderer->getCamera())
				mRenderer->getCamera()->onEvent(mEvent);
		
	}

	if(mRenderer->getGui() &&
		!mRenderer->getGui()->grabsFocus())
		processEvents();

	// mNet->loop();

	// Internally executes every n-th frame
	// Lua::getInstance()->loop();

	onPaint();

	return !mQuit;
}

void Window::onPaint()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mRenderer->paint();

	SDL_GL_SwapWindow(mWindow);
}

void Window::onResize(int width, int height)
{
	mScreenWidth = width;
	mScreenHeight = height;
	glViewport(0, 0, mScreenWidth, mScreenHeight);

	Log() << "Window: Resize width: " << width << " height: " << height;

	mRenderer->windowResized(mScreenWidth, mScreenHeight);

	// Lua::getInstance()->resizeWindow();
}

bool Window::onClick(int x, int y)
{
	if(!mRenderer->getGui()) return false;
	return mRenderer->getGui()->click(x, y);
}

bool Window::onDrag(int x, int y)
{
	if(!mRenderer->getGui()) return false;
	return mRenderer->getGui()->drag(x, y);
}

bool Window::onDrop(int x, int y)
{
	if(!mRenderer->getGui()) return false;
	return mRenderer->getGui()->drop(x, y);
}

