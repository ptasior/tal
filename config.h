#pragma once
#include <sol.hpp>

#define ANDROID_DATA_PATH "/sdcard/tal/"
#define CONFIG_FILE_LOCATION "game/config.lua"

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

