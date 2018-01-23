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
		Log(Log::DIE) << "Net: resolve host ("
					<< addr << ":"
					<< port << "): "
					<< SDLNet_GetError();

	if(!(mSock = SDLNet_TCP_Open(&mIp)))
		Log(Log::DIE) << "Net: open ("
					<< addr << ":"
					<< port << "): "
					<< SDLNet_GetError();

	if((mSocketSet = SDLNet_AllocSocketSet(1)) == NULL)
		Log(Log::DIE) << "Net: failed to allocate the socket set: " << SDLNet_GetError();

	SDLNet_TCP_AddSocket(mSocketSet, mSock);

	global_sharedData->setOnline(true);
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

	char recvbuf[BUFFSIZE+1] = {0};
	short lenr;

	if(SDLNet_CheckSockets(mSocketSet, 0) && SDLNet_SocketReady(mSock))
		if((lenr = SDLNet_TCP_Recv(mSock, recvbuf, BUFFSIZE)) > 0)
		{
			Log() << "Net: received: " << recvbuf << " len: " << lenr;
			int b = 0, e = 0;
			for(; e < lenr; e++)
				if(recvbuf[e] == '\2')
				{
					recvbuf[e] = '\0';
					if(!mRemainingBuff.empty())
					{
						Log() << "Net: applying with reminder: " << mRemainingBuff+(recvbuf+b);
						global_sharedData->applyChange(mRemainingBuff+(recvbuf+b));
						mRemainingBuff.clear();
					}
					else
						global_sharedData->applyChange(recvbuf+b);
					b = e+1;
				}

			if(e < lenr)
			{
				recvbuf[e+1] = '\0'; // There is still space
				mRemainingBuff = recvbuf;
			}
		}
		else
			Log() << "Net: sever error, lenr = " << lenr;

	// If nothing to send, return
	if(global_sharedData->getChanges().empty())
		return;
	std::string line = global_sharedData->getChanges().front()+'\2';
	global_sharedData->getChanges().pop();

	int actual = 0;
	int len = line.size(); // -1 removes \0
	// assert(len < BUFFSIZE); // Remote buffer size // Shouldn't be needed any more

	if((actual = SDLNet_TCP_Send(mSock, (void *)line.c_str(), len)) != len)
	{
		Log() << "Net: send: count: "
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

