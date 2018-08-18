#pragma once

#include <map>
#include <string>
#include <memory>

/*
 * Config c;
 * c.loadFile("Config.cfg");
 * c.loadString("aa = bb");
 *
 * std::string value = c.get("keyName");
 * int i = c.getInt("keyName");
 *
 * c.print();
*/

class StreamReader;

class Config
{
public:
	Config();

	std::string get(const std::string& key) const;
	int getInt(const std::string& key) const;
	bool getBool(const std::string& key) const;

	void load(std::shared_ptr<StreamReader> reader);
	void print() const;

protected:

	std::map<std::string, std::string> mData;
};

