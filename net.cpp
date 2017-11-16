#include "net.h"
#include "log.h"
#include <SDL_net.h>
#include <assert.h>
#include <errno.h>

Net::Net()
{
}

Net::~Net()
{
    if(mSock)
        disconnect();
}

void Net::connect()
{
    if(mSock) return;

    if(SDLNet_ResolveHost(&mIp, "10.0.0.2", 1234) < 0)
        Log(Log::DIE) << "SDLNet_ResolveHost:" << SDLNet_GetError();

    if(!(mSock = SDLNet_TCP_Open(&mIp)))
        Log(Log::DIE) << "SDLNet_TCP_Open:" << SDLNet_GetError();

    if((mSocketSet = SDLNet_AllocSocketSet(1)) == NULL)
        Log(Log::DIE) << "Failed to allocate the socket set: " << SDLNet_GetError();

    SDLNet_TCP_AddSocket(mSocketSet, mSock);

    Log() << "connected";
}

void Net::disconnect()
{
    SDLNet_TCP_DelSocket(mSocketSet, mSock);
    SDLNet_FreeSocketSet(mSocketSet);
    SDLNet_TCP_Close(mSock);
    Log() << "disconnected";
}

void Net::send(const std::string & msg)
{
    mSendMessage = msg;
}

void Net::loop()
{
    if(!mSock) return; //connect();

    char recvbuf[1024] = {0};

    if(SDLNet_CheckSockets(mSocketSet, 0) && SDLNet_SocketReady(mSock))
        if(SDLNet_TCP_Recv(mSock, recvbuf, 1024))
        {
            Log() << "received: " << recvbuf;
            mMessages.push_back(std::string(recvbuf));
        }

    // If nothing to send, return
    if(mSendMessage.empty())
        return;

    int actual = 0;
    int len = mSendMessage.size();
    assert(len < 1024); // Remote buffer size

    if((actual = SDLNet_TCP_Send(mSock, (void *)mSendMessage.c_str(), len)) != len)
    {
        Log() << "SDLNet_TCP_Send: count: "
              << actual << "/" << len
              << " err: " << SDLNet_GetError();

        if(errno == ENOTCONN)
        {
            Log() << "errno == ENOTCONN";
            #ifdef __EMSCRIPTEN__
            mSock = nullptr; // A bit hacky, but its for web
            #else
            disconnect();
            #endif
            connect();
            return;
        }
        if (errno != EAGAIN)
            Log(Log::DIE) << "errno != EAGAIN, " << errno;
        if(actual > 0)
            Log(Log::DIE) << "actual > 0";
    }
    else
    {
        Log() << "message sent: " << mSendMessage;
        mSendMessage = "";
    }
}

