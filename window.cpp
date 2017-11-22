#include "window.h"
#include "log.h"
#include "shader.h"
#include "model.h"
#include "sprite.h"
#ifdef __EMSCRIPTEN__
	#include <SDL_ttf.h>
#else
	#include <SDL2/SDL_ttf.h>
#endif


#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Model mModel;
Sprite mSprite;
Sprite mSprite2;
std::shared_ptr<Shader> mShader;

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
							SCREEN_WIDTH, SCREEN_HEIGHT,
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
		Log(Log::DIE) << "Cannot initaialise fonts " << TTF_GetError() << SDL_GetError();

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

	mModel.init();
	mSprite.init("assets/tex.png");
	mSprite2.init("assets/penguin.png");

	Log() << "Initialisation succesed";
}

Window::~Window()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
	// TODO check if TTF needs to be destroyerd too
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
					mNet.connect();
					mNet.send("new message");
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

	mNet.loop();
	onPaint();
}

void Window::onPaint()
{
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -4.0));
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 2.0, 0.0), glm::vec3(0.0, 0.0, -4.0), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f * SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 10.0f);
	// glm::mat4 mvp = projection * view * model;

	float angle = SDL_GetTicks() / 1000.0 * 45;  // 45° per second
	glm::vec3 axis_y(0, 1, 0);
	glm::mat4 anim = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis_y);
	glm::mat4 mvp = projection * view * model * anim;


	// Enable alpha
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// mModel.setMVP(mvp);
	mSprite.setMVP(mvp);
	glm::mat4 pos = glm::translate(glm::mat4(1.0f), glm::vec3(0.5, 0.0, 0.5));
	mSprite.setPosition(pos);

	mSprite.paint();

	// mSprite2.setMVP(mvp);
	glm::mat4 pos2 = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5, 0.0, 0.0));
		// * glm::rotate(glm::mat4(1.0f), glm::radians(1.7f), axis_y);
	mSprite2.setPosition(pos2);

	// mModel.paint();
	mSprite2.paint();

	SDL_GL_SwapWindow(mWindow);
}

void Window::onResize(int width, int height)
{
  SCREEN_WIDTH = width;
  SCREEN_HEIGHT = height;
  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

