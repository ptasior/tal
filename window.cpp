#include "window.h"
#include "log.h"
#include <SDL.h>

Window::Window()
{
	if(SDL_Init( SDL_INIT_VIDEO ) < 0)
		Log(Log::DIE) << "SDL could not initialize! SDL_Error: " << SDL_GetError();

	//Create window
	mWindow = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if(!mWindow)
		Log(Log::DIE) << "Window could not be created! SDL_Error: " << SDL_GetError();

	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (mRenderer == nullptr)
	{
		SDL_DestroyWindow(mWindow);
		std::string msg = std::string("SDL_CreateRenderer Error: ") + SDL_GetError();
		SDL_Quit();
		Log(Log::DIE) << msg;
	}



	// TODO Rubbish, remove later
	std::string imagePath = "assets/bmp.bmp";
	SDL_Surface *bmp = SDL_LoadBMP(imagePath.c_str());
	if (bmp == nullptr){
		SDL_DestroyRenderer(mRenderer);
		SDL_DestroyWindow(mWindow);
		Log() << "SDL_LoadBMP Error: " << SDL_GetError();
		SDL_Quit();
		return ;
	}

	tex = SDL_CreateTextureFromSurface(mRenderer, bmp);
	SDL_FreeSurface(bmp);
	if (tex == nullptr){
		SDL_DestroyRenderer(mRenderer);
		SDL_DestroyWindow(mWindow);
		Log()<< "SDL_CreateTextureFromSurface Error: " << SDL_GetError();
		SDL_Quit();
		return ;
	}

	Log() << "Initialisation succesed";
}

Window::~Window()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
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
					mNet.connect();
					mNet.send("new message");
					break;
			}
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
	static SDL_Event event; // That SHOULD be safe here...?

	if(SDL_PollEvent(&event))
		onEvent(event);

	onPaint();
	mNet.loop();
}

void Window::onPaint()
{
	// SDL_Rect dest = {.x = 200, .y = 100, .w = 200, .h = 200};
	// SDL_RenderCopy (mRenderer, tex, NULL, &dest);

	//A sleepy rendering loop, wait for 3 seconds and render and present the screen each time
	//First clear the renderer
	SDL_RenderClear(mRenderer);
	//Draw the texture
	SDL_RenderCopy(mRenderer, tex, NULL, NULL);
	//Update the screen
	SDL_RenderPresent(mRenderer);
	//Take a quick break after all that hard work
	// SDL_Delay(1000);

	// SDL_DestroyTexture(tex);
}

