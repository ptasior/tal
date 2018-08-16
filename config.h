#pragma once
#include <sol.hpp>

#define ANDROID_DATA_PATH "/sdcard/tal/"
#define CONFIG_FILE_LOCATION "config.lua"

class Config
{
public:
	Config();
	std::string get(std::string key) const;
	std::string get(std::string key, std::string defaultVal) const;
	bool getBool(std::string key) const;
	std::string operator[](std::string key);

protected:
	sol::state mState;
};

extern Config* global_config;

