#ifndef IO_H
#define IO_H

/*
	functions for handling input and output files
	also includes a class for storing data read from a file
*/


#include <string>
#include <vector>

namespace io
{

// CLASSES
class input_cl
{
	std::string 				Name;
	std::vector<std::string> 	Data;
	
	public:
		const std::string &name() const;
		std::vector<std::string> &data();
		input_cl(const std::string &n, const std::vector<std::string> &d);			// assign data to member data
};

// FUNCTIONS
void 	read_file(const char * file, std::vector<input_cl> &dest);			// read data from file <file> and store in <dest>
void 	split_line(const std::string &s, std::vector<std::string> &dest);		// split line on spaces and tabs, ignoring comments

} // end namespace io

#endif