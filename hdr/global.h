#ifndef GLOBAL_H
#define GLOBAL_H

#include <vector>
#include <string>
#include <sstream>

// PROTOTYPES

std::string make_lowercase(const std::string &s);

template <typename T>
T str_convert(const std::string &s);

template <typename T>
std::vector<T> str_convert(const std::vector<std::string> &s);

template<typename T>
std::vector<T> seq(const T start, const T end, const T by);



// TEMPLATE FUNCTIONS
template<typename T>
std::vector<T> seq(const T start, const T end, const T by)
{
	// returns a sequence in [start, end], incremented by by
	std::vector<T> result;
	for(T i = start; i <= end; i += by)
		result.push_back(i);

	return result;
}

template <typename T>
T str_convert(const std::string &s)
{
	// attempts to convert the string in s into a value of type T

	T result;
	std::istringstream val(s);			// create stream from the string
	if(!(val >> result)) {
		std::stringstream ss;
		ss << "Cannot convert value <" << s << "> from string into requested type";
		throw(ss.str());
	}
	return result;
}

template <typename T>
std::vector<T> str_convert(const std::vector<std::string> &s)
{
	// converts a vector of strings into a vector of T
	
	std::vector<T> result;
	for(int i = 0; i < s.size(); i++)
		result.push_back(str_convert<T>(s.at(i)));

	return result;
}

template <typename T>
std::string convert_to_str(T x)
{
	// convert the object x to a string and return the resulting string
	std::string result;
	std::stringstream res;
	res << x;
	result = res.str();
	return(result);
}

template <typename T>
std::vector<std::string> convert_to_str(std::vector<T> x)
{
	// convert a vector<T> into a vector<string>
	std::vector<std::string> result;
	for(int i = 0; i < x.size(); i++)
		result.push_back( convert_to_str(x[i]) );

	return(result);
}

#endif
