#pragma once
#include <SDL_net.h> // TODO Move it to cpp
#include <string>
#include <vector>

class Net
{
public:
    Net();
    ~Net();

    void connect();
    void disconnect();

    void loop();

private:
    IPaddress mIp;
    TCPsocket mSock = nullptr;
    SDLNet_SocketSet mSocketSet = nullptr;
};

