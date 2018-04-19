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
		// write("---Fatal-error--------------------------");
		// write(Log::endl);
	}
}

Log::~Log()
{
	write(Log::endl);
	if(mAction == DIE)
	{
		// write("----------------------------------------");
		write(Log::endl);
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
	if(mAction == ERR || mAction == DIE)
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", str, NULL);


	// 	std::cerr << str;
	// else
	// 	std::cout << str;

#ifdef ANDROID
	__android_log_print(ANDROID_LOG_DEBUG, "tal", "%s", str);
#endif
}

void Log::flush()
{
	// std::cout << std::flush;
}

