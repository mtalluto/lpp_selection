#ifndef MAIN_H
#define MAIN_H

#include "iterator.h"
#include <vector>
#include <string>

// prototypes
struct gl_settings;
void handle_args(int argc, char * argv [], gl_settings & settings);
void write_output(std::vector<std::string> & outData, const char * file, iteration_step & parData, bool header, bool writeParams = true);
void chomp(std::string & data);

struct gl_settings
{
	size_t numThreads, numReps;
	std::string parFile, iterFile, outFile;
	
	gl_settings() : numThreads(1), numReps(1), parFile("parameters.txt"), iterFile("iteration.txt"), outFile("population_history.csv") {}
};

#endif