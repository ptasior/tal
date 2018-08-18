#pragma once

#include <map>
#include <string>

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

class Config
{
public:
	Config();

	std::string get(const std::string& key) const;
	int getInt(const std::string& key) const;
	bool getBool(const std::string& key) const;

	void loadString(const std::string& data);
	void loadFile(const std::string& name);

	void print() const;

protected:
	void load(std::istream *data);

	std::map<std::string, std::string> mData;
};

