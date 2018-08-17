#include "config.h"
#include "global.h"
#include "data_reader.h"
#include "log.h"

Config::Config()
{
	std::string data = Global::get<DataReader>()->readString(CONFIG_FILE_LOCATION);
	auto r = mState.script(data);

	if(!r.valid())
		Log(Log::DIE) << "Config: Error while loading " CONFIG_FILE_LOCATION;

	Log() << "Config: Initialised";
}

std::string Config::get(std::string key) const
{
	// std::string t = mState[key.c_str()];
	// Log() << "Config: key: " << key << " = " << t;
	return mState[key.c_str()].get<std::string>();
}

std::string Config::get(std::string key, std::string defaultVal) const
{
	auto t = mState[key.c_str()];
	// Log() << "Config (default): key: " << key << " = " << t.get<std::string>();
	return t != sol::nil ? t.get<std::string>() : defaultVal;
}

bool Config::getBool(std::string key) const
{
	// std::string t = mState[key.c_str()];
	// Log() << "Config (bool): key: " << key << " = " << t;
	return mState[key.c_str()].get<bool>();
}

std::string Config::operator[](std::string key)
{
	return mState[key.c_str()];
}

