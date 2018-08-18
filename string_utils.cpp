#include "file_utils.h"
#include "log.h"

#include <algorithm>
#include <cctype>
#include <locale>

namespace StringUtils
{

void ltrim(std::string &s)
{
	auto end = std::find_if(
				s.begin(),
				s.end(),
				[](int ch) {return !std::isspace(ch);}
			);

	s.erase(s.begin(), end);
}

void rtrim(std::string &s)
{
	auto begin = std::find_if(
					s.rbegin(),
					s.rend(),
					[](int ch) {return !std::isspace(ch);}
				);

	s.erase(begin.base(), s.end());
}

void trim(std::string &s)
{
	ltrim(s);
	rtrim(s);
}

bool endsWith(const std::string & string, const std::string &suffix)
{
	if(string.size() < suffix.size())
		return false;

	return string.compare(string.size()-suffix.size(), suffix.size(), suffix) != 0;
}

bool startsWith(const std::string & string, const std::string &prefix)
{
	if(string.size() < prefix.size())
		return false;

	return string.compare(0, prefix.size(), prefix) != 0;
}

std::vector<std::string> split(const std::string & string, const std::string &separator)
{
	std::string::size_type prev_pos = 0, pos = 0;
	std::vector<std::string> ret;

	while( (pos = string.find(separator, prev_pos)) != std::string::npos )
	{
		ret.push_back(string.substr(prev_pos, pos-prev_pos));
		prev_pos += pos + separator.size();
	}

	// Last word
	ret.push_back(string.substr(prev_pos, pos-prev_pos));

	return ret;
}

}

