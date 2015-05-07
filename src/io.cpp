#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "../hdr/io.h"

using namespace io;

	/*-----------------------
	|						|
	|	    input_cl		|
	|						|
	-----------------------*/	

const std::string &input_cl::name() const
{ return Name; }

std::vector<std::string> &input_cl::data()
{ return Data; }

	/*-----------------------
	|						|
	|	   functions		|
	|						|
	-----------------------*/	

void io::read_file(const char * file, std::vector<input_cl> &dest)
{
	// open input file
	std::ifstream inputFile (file);
	if(!inputFile.is_open())
		throw( std::string("Cannot open file <" + std::string(file) + ">" ));
		
	while(inputFile.good())
	{
		static size_t lineno = 0;								// for error reporting
		std::string line;
		getline(inputFile, line);
		lineno++;
		
		std::vector<std::string> lineData;								// data from this line
		std::string varName;											// variable name from this line
		try {
			split_line(line, lineData);							// split the input data
			if(lineData.size() == 0)	continue;				// skip empty/comment lines
			varName = lineData.at(0);							// grab the variable name
			lineData.erase(lineData.begin());					// delete the variable name from the data
		}
		catch (...) {
			std::stringstream ss;
			ss <<  "Problem reading from input file, line " << lineno << "\n\t" << line;
			throw( ss.str() );		
		}
		
		dest.push_back( input_cl (varName, lineData) );
	}
	inputFile.close();
}

void io::split_line(const std::string &s, std::vector<std::string> &dest)
/*
	takes an input line l and splits it on spaces and tabs
	comments prefaced with # are ignored
	results are placed into a vector of strings named dest
*/
{
	std::stringstream ls(s);		// create stream out of input data
	std::string data;
	while(getline(ls, data, ' '))		// split ls into data on spaces
	{
		std::stringstream ds(data);			// create another stream on data
		while(getline(ds, data, '\t'))	// split on tabs as well
		{
			if(data.at(0) == '#')		// ignore comments
				break;
			if(data.size() > 0)
				dest.push_back(data);
		}
		if(data.at(0) == '#')		// ignore comments
			break;
	}
}

	/*-----------------------
	|						|
	|	   constructors		|
	|						|
	-----------------------*/	

input_cl::input_cl(const std::string &n, const std::vector<std::string> &d) :
Name (n), Data (d)
{ }