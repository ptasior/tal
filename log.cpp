#include "log.h"
#include <iostream>

Log::Log()
{}

Log::~Log()
{
    // write("\n");
    std::cout << std::endl;
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

Log& Log::operator << (int val)
{
    // TODO Use atoi
    auto tmp = std::to_string(val);
    write(tmp.c_str());
    return *this;
}


void Log::write(const char* str)
{
    std::cout << str;
}

