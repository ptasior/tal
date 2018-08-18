#pragma once
#include <string>
#include <vector>

namespace StringUtils
{

void ltrim(std::string &s);
void rtrim(std::string &s);
void trim(std::string &s);

bool endsWith(const std::string & string, const std::string &suffix);
bool startsWith(const std::string & string, const std::string &prefix);

std::vector<std::string> split(const std::string & string, const std::string &separator);

}

