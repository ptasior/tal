#pragma once
#include <sol.hpp>

class Config
{
public:
	Config();
	std::string get(std::string key);
	std::string operator[](std::string key);

protected:
	sol::state mState;
};

extern Config* global_config;

