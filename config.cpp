#include "config.h"
#include "config_default.h"
#include "string_utils.h"
#include "log.h"
#include <fstream>
#include <sstream>

Config::Config()
{
	loadString(EmbeddedData::config_default);
}

void Config::loadFile(const std::string& name)
{
	Log() << "Config: Loading: " << name;
	std::ifstream file(name);
	load(&file);
}

void Config::loadString(const std::string& data)
{
	std::stringstream stream(data);
	load(stream);
	Log() << "Config: Loding string";
}

void Config::load(std::istream *data)
{
	std::string line;

	int cnt = 0;

	while(data->good())
	{
		std::getline(*data, line);
		StringUtils::trim(line);
		cnt++;

		if(line.empty() || line[0] == '#') continue;
		
		auto kv = StringUtils::split(line, "=");
		if(kv.size() != 2)
			Log(Log::DIE) << "Config: Incorrect syntax in line: " << cnt
							<< Log::endl << "  " << line;

		StringUtils::rtrim(kv[0]);
		StringUtils::ltrim(kv[1]);

		if(kv[0].empty() || kv[1].empty())
			Log(Log::DIE) << "Config: Incorrect key or value in line: " << cnt
							<< Log::endl << "  " << line;

		mData[kv[0]] = kv[1];
	}
}

std::string Config::get(const std::string& key) const
{
	if(!mData.count(key))
		Log(Log::DIE) << "Config: key: " << key << " does not exist";
	return mData.at(key);
}

bool Config::getBool(const std::string& key) const
{
	return get(key) == "true";
}

int Config::getInt(const std::string& key) const
{
	return std::stoi(get(key));
}

void Config::print() const
{
	for(auto kv : mData)
		Log() << kv.first << " = " << kv.second;
}

