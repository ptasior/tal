#pragma once
#include <selene.h>

class Config
{
public:
	Config();
	std::string get(std::string key);
	std::string operator[](std::string key);

protected:
	sel::State mState;
};

extern Config* global_config;

