#include "net.h"
#include "log.h"
#include "shared_data.h"
#include "config.h"
#include "global.h"
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

	std::string addr = Global::get<Config>()->get("serverAddr");
	int port;
	#ifdef JS
		port = std::stoi(Global::get<Config>()->get("serverWSPort"));
	#else
		port = std::stoi(Global::get<Config>()->get("serverPort"));
	#endif
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

	Global::get<SharedData>()->setOnline(true);
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
	{
		if((lenr = SDLNet_TCP_Recv(mSock, recvbuf, BUFFSIZE)) > 0)
		{
			// Log() << "Net: received: " << recvbuf << " len: " << lenr;
			int b = 0, e = 0;
			for(; e < lenr; e++)
				if(recvbuf[e] == '\2')
				{
					recvbuf[e] = '\0';
					if(!mRemainingBuff.empty())
					{
						Log() << "Net: applying with reminder: " << mRemainingBuff+(recvbuf+b);
						Global::get<SharedData>()->applyChange(mRemainingBuff+(recvbuf+b));
						mRemainingBuff.clear();
					}
					else
						Global::get<SharedData>()->applyChange(recvbuf+b);
					b = e+1;
				}

			if(e < lenr)
			{
				recvbuf[e+1] = '\0'; // There is still space
				mRemainingBuff = recvbuf;
			}
		}
		else
			Log(Log::DIE) << "Net: sever error, lenr = " << lenr;
	}

	// If nothing to send, return
	if(Global::get<SharedData>()->getChanges().empty())
		return;
	std::string line = Global::get<SharedData>()->getChanges().front()+'\2';
	Global::get<SharedData>()->getChanges().pop();

	int actual = 0;
	int len = line.size();

	if((actual = SDLNet_TCP_Send(mSock, (void *)line.c_str(), len)) != len)
	{
		Log() << "Net: send: count: "
			  << actual << "/" << len
			  << " err: " << SDLNet_GetError();

		if(errno == ENOTCONN)
		{
			Log() << "Net: errno == ENOTCONN";
			#ifdef JS
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
	// else
	// 	Log() << "Net: message sent: " << line;
}

