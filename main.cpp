//Using SDL and standard IO
#include <SDL.h>
#include <SDL_net.h>

#include <stdio.h>
#include "log.h"
#include <errno.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


// void net()
// {
//     const char *host;
// 
//     IPaddress serverIP;
//     TCPsocket clientSocket;
//     std::string  serverName = "10.0.0.2";
// 
//     const char *buffer = "my message";
// 
// 
//     SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(1);
//     if (socketSet == NULL)
//         Log(Log::DIE) << "Failed to allocate the socket set: " << SDLNet_GetError();
//     else
//         Log() << "Successfully allocated socket set";
// 
//     // Try to resolve the host. If successful, this places the connection details in the serverIP object
//     int hostResolved = SDLNet_ResolveHost(&serverIP, serverName.c_str(), 1234);
// 
//     if (hostResolved == -1)
//         Log() << "Failed to resolve the server hostname: " << SDLNet_GetError() << "\nContinuing...\n";
//     else // If we successfully resolved the host then output the details
//     {
//         // Get our IP address in proper dot-quad format by breaking up the 32-bit unsigned host address and splitting it into an array of four 8-bit unsigned numbers...
//         Uint8 * dotQuad = (Uint8*)&serverIP.host;
// 
//         //... and then outputting them cast to integers. Then read the last 16 bits of the serverIP object to get the port number
//         Log() << "Successfully resolved host to IP: " << (unsigned short)dotQuad[0] << "." << (unsigned short)dotQuad[1] << "." << (unsigned short)dotQuad[2] << "." << (unsigned short)dotQuad[3];
//         Log() << " port " << SDLNet_Read16(&serverIP.port) << Log::endl;
//     }
// 
//     // // Try to resolve the IP of the server, just for kicks
//     // if ((host = SDLNet_ResolveIP(&serverIP)) == NULL)
//     // {
//     //     Log() << "Failed to resolve the server IP address: " << SDLNet_GetError();
//     // }
//     // else
//     // {
//     //     Log()<< "Successfully resolved IP to host: " << host;
//     // }
// 
//     // Flag to keep track of when to disconnect and finish up. We initially set it so that we CANNOT connect, and only change this to false when we got an "OK" response from the server
// 
//     // Try to open a connection to the server and quit out if we can't connect
//     clientSocket = SDLNet_TCP_Open(&serverIP);
//     if (!clientSocket)
//         Log(Log::DIE) << "Failed to open socket to server: " << SDLNet_GetError();
// 
//     Log() << "Connection okay, about to read connection status from the server...";
// 
//     // Add our socket to the socket set for polling
//     SDLNet_TCP_AddSocket(socketSet, clientSocket);
// 
//     // Wait for up to five seconds for a response from the server
//     // Note: If we don't check the socket set and WAIT for the response, we'll be checking before the server can respond, and it'll look as if the server sent us nothing back
//     int activeSockets = SDLNet_CheckSockets(socketSet, 5000);
// 
//     Log() << "There are " << activeSockets << " socket(s) with data on them at the moment.";
// 
//     // Check if we got a response from the server
//     int gotServerResponse = SDLNet_SocketReady(clientSocket);
// 
//     if (gotServerResponse != 0)
//     {
//         Log() << "Got a response from the server... ";
//         char buf[1024];
//         int serverResponseByteCount = SDLNet_TCP_Recv(clientSocket, buf, 1024);
// 
//         Log() << "Got the following from server: " << buf << "(" << serverResponseByteCount << " bytes)";
//     }
//     else
//     {
//         Log()<< "No response from server...";
//     }
// 
//     // Send the message to the server
//     int len = strlen(buffer);
//     if(SDLNet_TCP_Send(clientSocket, (void *)buffer, len) < len)
//         Log(Log::DIE) << "Failed to send message: " << SDLNet_GetError();
// 
//     Log() << "message was sent!" << SDLNet_GetError();
// 
//     char buf[1024];
//     int serverResponseByteCount = SDLNet_TCP_Recv(clientSocket, buf, 1024);
//     Log() << "sever resp: " << buf;
// 
//     // // Check our socket set for activity. Don't wait if there's nothing on the socket just continue
//     // int socketActive = SDLNet_CheckSockets(socketSet, 0);
// 
//     // //cout << "Sockets with data on them at the moment: " << activeSockets << endl;
// 
//     // if (socketActive != 0)
//     // {
//     //     // Check if we got a response from the server
//     //     int messageFromServer = SDLNet_SocketReady(clientSocket);
// 
//     //     if (messageFromServer != 0)
//     //     {
//     //         //cout << "Got a response from the server... " << endl;
//     //         int serverResponseByteCount = SDLNet_TCP_Recv(clientSocket, buffer, BUFFER_SIZE);
// 
//     //         Log()<< "Received: " << buffer;
// 
//     //         if (strcmp(buffer, "shutdown") == 0)
//     //         {
//     //             Log()<< "Server is going down. Disconnecting...";
//     //             shutdownClient = true;
//     //         }
//     //     }
//     //     else
//     //     {
//     //         //cout << "No response from server..." << endl;
//     //     }
// 
//     // } // End of if socket has activity check
// 
//     SDLNet_TCP_Close(clientSocket);
// }

class Window
{
public:
    Window()
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


        // TODO Move to nettwork class
        if(SDLNet_ResolveHost(&ip, "10.0.0.2", 1234) < 0)
            Log(Log::DIE) << "SDLNet_ResolveHost:" << SDLNet_GetError();

        if(!(sock = SDLNet_TCP_Open(&ip)))
            Log(Log::DIE) << "SDLNet_TCP_Open:" << SDLNet_GetError();

        if((socketSet = SDLNet_AllocSocketSet(1)) == NULL)
            Log(Log::DIE) << "Failed to allocate the socket set: " << SDLNet_GetError();

        SDLNet_TCP_AddSocket(socketSet, sock);

        Log() << "Initialisation succesed";
    }

    ~Window()
    {
        SDL_DestroyRenderer(mRenderer);
        SDL_DestroyWindow(mWindow);
        SDL_Quit();
        Log() << "Quit";
    }

    void net()
    {
        int actual = 0, len;
        char recvbuf[1024] = {0};

        if(!sendMessage.empty())
            if((actual = SDLNet_TCP_Send(sock, (void *)sendMessage.c_str(), sendMessage.size())) != sendMessage.size())
            {
                Log() << "SDLNet_TCP_Send: count: "
                      << actual << "/" << sendMessage.size()
                      << " err: " << SDLNet_GetError();
                // TODO if errno == 107 reconnect
                if (errno != EAGAIN)
                    Log(Log::DIE) << "errno != EAGAIN, " << errno;
                if(actual > 0)
                    Log(Log::DIE) << "actual > 0";
            }
            else
            {
                Log() << "message sent";
                sendMessage = "";
            }

        auto a = SDLNet_CheckSockets(socketSet, 0);
        auto b = SDLNet_SocketReady(sock);
        Log() << "a: " << a << " b: " << b;
        if(a && b)
        // if(SDLNet_CheckSockets(socketSet, 0) && SDLNet_SocketReady(sock))
        {
            if(SDLNet_TCP_Recv(sock, recvbuf, 1024))
                Log() << "received: " << recvbuf;
        }
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
                case SDLK_n:
                        Log() << "net";
                        sendMessage = "my text";
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
        net();
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

    IPaddress ip;
    TCPsocket sock;
    SDLNet_SocketSet socketSet;
    std::string sendMessage;
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

