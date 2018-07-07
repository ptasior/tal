#include "log.h"
#include <iostream>
#include <SDL.h>

#ifdef JS
	#include <emscripten.h>
#endif

#ifdef ANDROID
	#include <android/log.h>
#endif

const char *Log::endl = "\n";

Log::Log(Action a):
	mAction(a)
{
	if(mAction == DIE)
	{
		std::cerr << "---Fatal-error--------------------------" << std::endl;
	}
}

Log::~Log()
{
	flush();

	if(mAction == DIE)
	{
		std::cerr << "----------------------------------------" << std::endl;
		#ifdef JS
		emscripten_cancel_main_loop();
		#endif
		std::exit(EXIT_FAILURE);
	}
}

Log& Log::operator << (const std::string & str)
{
	write(str.c_str());
	return *this;
}

Log& Log::operator << (const char* str)
{
	write(str);
	return *this;
}

Log& Log::operator << (double val)
{
	// TODO Use atoi
	auto tmp = std::to_string(val);
	write(tmp.c_str());
	return *this;
}

Log& Log::operator << (float val)
{
	// TODO Use atoi
	auto tmp = std::to_string(val);
	write(tmp.c_str());
	return *this;
}

Log& Log::operator << (unsigned long val)
{
	// TODO Use atoi
	auto tmp = std::to_string(val);
	write(tmp.c_str());
	return *this;
}

Log& Log::operator << (long val)
{
	// TODO Use atoi
	auto tmp = std::to_string(val);
	write(tmp.c_str());
	return *this;
}

Log& Log::operator << (unsigned int val)
{
	// TODO Use atoi
	auto tmp = std::to_string(val);
	write(tmp.c_str());
	return *this;
}

Log& Log::operator << (int val)
{
	// TODO Use atoi
	auto tmp = std::to_string(val);
	write(tmp.c_str());
	return *this;
}

Log& Log::operator << (char val)
{
	char tmp[] = {val, '\0'};
	write(tmp);
	return *this;
}

void Log::write(const char* str)
{
	mMessage += str;
}

void Log::flush()
{
#ifdef ANDROID
	__android_log_print(ANDROID_LOG_DEBUG, "tal", "%s", mMessage.c_str());
#else
	if(mAction == ERR || mAction == DIE)
		std::cerr << mMessage;
	else
		std::cout << mMessage;

	std::cout << std::endl;
#endif

	if(mAction == ERR || mAction == DIE)
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", mMessage.c_str(), NULL);
}

