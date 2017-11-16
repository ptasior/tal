//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>
#include "log.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


class Window
{
public:
    Window()
    {
        if(SDL_Init( SDL_INIT_VIDEO ) < 0)
            die(std::string("SDL could not initialize! SDL_Error: ") + SDL_GetError());

        //Create window
        mWindow = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        if(!mWindow)
            die(std::string("Window could not be created! SDL_Error: ") + SDL_GetError());

        mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (mRenderer == nullptr)
        {
            SDL_DestroyWindow(mWindow);
            std::string msg = std::string("SDL_CreateRenderer Error: ") + SDL_GetError();
            SDL_Quit();
            die(msg);
        }



        // TODO Rubbish, remove later
        std::string imagePath = "img/bmp.bmp";
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

        Log() << "ok";
    }

    ~Window()
    {
        SDL_DestroyRenderer(mRenderer);
        SDL_DestroyWindow(mWindow);
        SDL_Quit();
        Log() << "Quit";
    }

    void die(const std::string & msg)
    {
        Log() << msg;
        std::exit(EXIT_FAILURE);
    }

    void onEvent(SDL_Event &event)
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
                }
                break;
        }
    }

    void loop()
    {
        while (!mQuit)
            onLoop();
    }

    void onLoop()
    {
        static SDL_Event event; // That SHOULD be safe here...?

        if(SDL_PollEvent(&event))
            onEvent(event);

        onPaint();
    }

    void onPaint()
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

    //Screen dimension constants
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;

private:
    SDL_Window* mWindow = nullptr;
    SDL_Renderer *mRenderer = nullptr;

    SDL_Texture *tex = nullptr;
    bool mQuit = false;
};

#ifdef __EMSCRIPTEN__
Window *ptr_w;
void main_loop()
{
    ptr_w->onLoop();
}
#endif

int main( int argc, char* args[] )
{
    Window w;

    #ifdef __EMSCRIPTEN__
        ptr_w = &w;
        Log() << "Running Emscripten loop";
        emscripten_set_main_loop(main_loop, 60, true);
    #else
        w.loop();
    #endif

    return 0;
}

