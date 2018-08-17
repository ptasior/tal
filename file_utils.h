#pragma once
#include <vector>
#include <string>

namespace FileUtils
{
	std::vector<std::string> listDir(const std::string& dir, const std::string& suffix="");
	bool fileExists(const std::string& name);

	std::string dataFolderLocation();
}

