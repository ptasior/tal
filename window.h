#include "net.h"

class Window
{
public:
    Window();
    ~Window();

    void onEvent(SDL_Event &event);
    void onLoop();
    void onPaint();

    void loop();

    //Screen dimension constants
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;

private:
    SDL_Window* mWindow = nullptr;
    SDL_Renderer *mRenderer = nullptr;

    SDL_Texture *tex = nullptr;
    bool mQuit = false;

    Net mNet;
};

