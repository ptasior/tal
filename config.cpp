#include "config.h"
#include "log.h"

Config::Config()
{
	sol::load_result r = mState.load_file(CONFIG_FILE_LOCATION);
	if(!r.valid())
		Log(Log::DIE) << "Config: Error while loading " CONFIG_FILE_LOCATION;

	r();
	Log() << "Config: Initialised";
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

