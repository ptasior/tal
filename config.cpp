#include "config.h"
#include "log.h"

Config::Config()
{
	sol::load_result r = mState.load_file("game/config.lua");
	if(!r.valid())
		Log(Log::DIE) << "Error while loading config.lua";

	r();
	Log() << "Config initialised";
}

std::string Config::get(std::string key)
{
	std::string t = mState[key.c_str()];
	// Log() << "Config: key: " << key << " = " << t;
	return mState[key.c_str()];
}

std::string Config::operator[](std::string key)
{
	return mState[key.c_str()];
}

