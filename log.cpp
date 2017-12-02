#include "log.h"
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


const char *Log::endl = "\n";

Log::Log(Action a):
    mAction(a)
{}

Log::~Log()
{
    write(Log::endl);
    if(mAction == DIE)
    {
        #ifdef __EMSCRIPTEN__
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

Log& Log::operator << (int val)
{
    // TODO Use atoi
    auto tmp = std::to_string(val);
    write(tmp.c_str());
    return *this;
}


void Log::write(const char* str)
{
    if(mAction == ERR)
        std::cerr << str;
    else
        std::cout << str;
}

