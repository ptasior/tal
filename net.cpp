#include "net.h"
#include "log.h"
#include "shared_data.h"
#include "config.h"
#include <SDL_net.h>
#include <assert.h>
#include <errno.h>

const int BUFFSIZE = 2048;

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

	std::string addr = global_config->get("serverAddr");
	int port = std::stoi(global_config->get("serverPort"));
	if(SDLNet_ResolveHost(&mIp, addr.c_str(), port) < 0)
		Log(Log::DIE) << "Net: SDLNet_ResolveHost ("
					<< addr << ":"
					<< port << "): "
					<< SDLNet_GetError();

	if(!(mSock = SDLNet_TCP_Open(&mIp)))
		Log(Log::DIE) << "Net: SDLNet_TCP_Open ("
					<< addr << ":"
					<< port << "): "
					<< SDLNet_GetError();

	if((mSocketSet = SDLNet_AllocSocketSet(1)) == NULL)
		Log(Log::DIE) << "Net: Failed to allocate the socket set: " << SDLNet_GetError();

	SDLNet_TCP_AddSocket(mSocketSet, mSock);

	SharedData::setOnline(true);
	Log() << "Net: connected";
}

void Net::disconnect()
{
	SDLNet_TCP_DelSocket(mSocketSet, mSock);
	SDLNet_FreeSocketSet(mSocketSet);
	SDLNet_TCP_Close(mSock);
	Log() << "Net: disconnected";
}

void Net::loop()
{
	if(!mSock) return; //connect();

	char recvbuf[BUFFSIZE] = {0};

	if(SDLNet_CheckSockets(mSocketSet, 0) && SDLNet_SocketReady(mSock))
		if(SDLNet_TCP_Recv(mSock, recvbuf, BUFFSIZE))
		{
			Log() << "Net: received: " << recvbuf;
			SharedData::applyChange(recvbuf);
		}

	// If nothing to send, return
	if(SharedData::getChanges().empty())
		return;
	std::string line = SharedData::getChanges().front();
	SharedData::getChanges().pop();

	int actual = 0;
	int len = line.size();
	assert(len < BUFFSIZE); // Remote buffer size

	if((actual = SDLNet_TCP_Send(mSock, (void *)line.c_str(), len)) != len)
	{
		Log() << "Net: SDLNet_TCP_Send: count: "
			  << actual << "/" << len
			  << " err: " << SDLNet_GetError();

		if(errno == ENOTCONN)
		{
			Log() << "Net: errno == ENOTCONN";
			#ifdef __EMSCRIPTEN__
			mSock = nullptr; // A bit hacky, but its for web
			#else
			disconnect();
			#endif
			connect();
			return;
		}
		if (errno != EAGAIN)
			Log(Log::DIE) << "Net: errno != EAGAIN, " << errno;
		if(actual > 0)
			Log(Log::DIE) << "Net: actual > 0";
	}
	else
	{
		Log() << "Net: message sent: " << line;
	}
}

