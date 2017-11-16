#pragma once
#include <string>

class Log
{
public:
    Log();
    ~Log();

    Log& operator << (const std::string & str);
    Log& operator << (const char* str);
    Log& operator << (int val);

private:
    void write(const char* str);
};

