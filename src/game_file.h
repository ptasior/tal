#pragma once

#include "stream_reader.h"
#include <fstream>
#include <map>
#include <vector>
#include <memory>

/**
	DataReader r("../qq.dat");
	Log() << r.read("lua_lib/var_dump.lua");
*/

class StreamReaer;

class GameFile
{
public:
	GameFile(std::shared_ptr<StreamReader> reader);
	~GameFile();

	std::vector<char> readData(std::string file) const;
	std::string readString(std::string file) const;
	std::shared_ptr<StreamReader> readStream(std::string file) const;

	bool hasFile(const std::string& name) const;

	std::string name() const;
	void print() const;

private:
	struct Pos
	{
		uint32_t position;
		uint32_t size;
	};

	std::map<std::string, Pos> m_positions;
	std::shared_ptr<StreamReader> mStreamReader;
};

