#pragma once

#include <fstream>
#include <map>

/**
	DataReader r("../qq.dat");
	Log() << r.read("lua_lib/var_dump.lua");
*/

class DataReader
{
public:
	DataReader(std::string name);
	~DataReader();

	std::string read(std::string file) const;

private:
	struct Pos
	{
		uint32_t position;
		uint32_t size;
	};

	std::map<std::string, Pos> m_positions;
	mutable std::ifstream m_file;
};

