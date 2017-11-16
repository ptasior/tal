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

    void send(const std::string & msg);

    void loop();

private:
    IPaddress mIp;
    TCPsocket mSock = nullptr;
    SDLNet_SocketSet mSocketSet = nullptr;
    std::string mSendMessage;
    std::vector<std::string> mMessages;
};

