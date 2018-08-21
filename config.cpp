#include "config.h"
#include "string_utils.h"
#include "stream_reader.h"
#include "log.h"
#include <fstream>
#include <sstream>

Config::Config()
{}

void Config::load(std::shared_ptr<StreamReader> reader)
{
	if(!reader)
		Log(Log::DIE) << "Config: Empty reader";

	std::istream *data = reader->get();
	std::string line;

	int cnt = 0;

	while(data->good())
	{
		std::getline(*data, line);
		StringUtils::trim(line);
		cnt++;

		// Log() << cnt << ": " << line;

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

bool Config::has(const std::string& key) const
{
	return mData.count(key);
}
