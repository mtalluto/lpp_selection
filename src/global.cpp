#include "../hdr/global.h"

std::string make_lowercase(const std::string &s) 
{
	std::string result;
	for(int i = 0; i < s.size(); i++)
		result.push_back(tolower(s.at(i)));
	return result;
}