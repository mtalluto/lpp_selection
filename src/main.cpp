#include <iostream>
#include <fstream>
#include "../hdr/main.h"
#include "../hdr/landscape.h"
#include "../hdr/params.h"
#include "../hdr/global.h"
#include "../hdr/stats.h"

int main(int argc, char * argv [])
{
	// set up some default settings
	gl_settings settings;
	
	landscape_cl landscape;
	iterator_cl iterator;
	stats_cl stats;
	try {
		// deal with command line arguments
		handle_args(argc, argv, settings);
		
		// set up iteration
		iterator = iterator_cl (settings.iterFile, stats);
	
	}
	catch(std::string s) {
		std::cerr << s << std::endl;
		return(-1);
	}
	catch(...) {
		std::cerr << "An unknown exception occurred while setting up the model" << std::endl;
		return(-1);
	}

	parameters_cl params = parameters_cl(settings.parFile.c_str());
	settings.numReps = params.num_reps();
		
	// run the model
	bool writeHeader = true;
	int combsDone = 0;
	while(iterator.ready() ) {
		try{
			// create a parameter object
			params = parameters_cl(settings.parFile.c_str());
		}
		catch(std::string s) {
			std::cerr << s << std::endl;
			return(-1);
		}
		catch(...) {
			std::cerr << "An unknown exception occurred while reading parameters" << std::endl;
			return(-1);
		}
		
		// update the iterator
		iteration_step curParameters = iterator.update(params);

		// repeat this iteration
		for(int rep = 0; rep < settings.numReps; rep++) {
			// create the landscape
			landscape = landscape_cl(params, &stats);
	
			// set up container for output
			std::vector<std::string> popOutput, fireOutput;
	
			// run the model
			landscape.start(popOutput, fireOutput);
			
			write_output(popOutput, settings.outFile.c_str(), curParameters, writeHeader);
			// write_output(fireOutput, "fire_history.csv", curParameters, writeHeader, false); // disabled because we don't really need this
			if(writeHeader) writeHeader = false;
			combsDone++;
			if(combsDone % 5000 == 0 && combsDone > 0)
				std::cout << "Finished run " << combsDone << ", " << iterator.size()*settings.numReps << " remain\n";
		}
	}
	
	return(0);
}

void write_output(std::vector<std::string> & outData, const char * file, iteration_step & parData, bool header, bool writeParams)
{
	// open the output file
	std::ofstream outputFile;
	if(header) {
		outputFile.open(file);			// overwites any old files that might be there
	}
	else {
		outputFile.open(file, std::ios::app);	// open for appending
	}
	if(!outputFile.is_open()) throw(std::string("Could not open file " + std::string(file) + " for output"));
	
	std::vector<std::string>::iterator curLine = outData.begin();
	if(header) {
		// assume there is a header and write it out
		std::string outString = *curLine;
		if(writeParams && (parData.stepData.size() > 0)) {
			chomp(outString);
			for(int i = 0; i < parData.stepData.size(); i++) {
				outString += ",";
				outString += parData.stepData[i].name();
			}
			outString += "\n";
		}
		outputFile << outString;
	}
	
	curLine++;			// skip the header
	while(curLine != outData.end()) {
		// write out the data
		std::string outString = *curLine;
		if(writeParams && (parData.stepData.size() > 0)) {
			chomp(outString);
			for(int i = 0; i < parData.stepData.size(); i++) {
				outString += ",";
				outString += parData.stepData[i].data()[0];
			}
			outString += "\n";
		}
		outputFile << outString;
		curLine++;
	}
}

void chomp(std::string & data) 
{
	if(data[data.size() - 1] == '\n') {
		data = data.substr(0, data.size()-1);
		chomp(data);
	}
}

void handle_args(int argc, char * argv [], gl_settings & settings)
{
	for(int i = 0; i < argc; i++) {
		std::string thisArg = argv[i];
		if(thisArg == "-p") {
			i++;
			if(i >= argc) throw(std::string("Parameter file required after -p"));
			settings.parFile = argv[i];
		}
		if(thisArg == "-i") {
			i++;
			if(i >= argc) throw(std::string("Parameter file required after -i"));
			settings.iterFile = argv[i];			
		}
		if(thisArg == "-o") {
			i++;
			if(i >= argc) throw(std::string("Output file required after -o"));
			settings.outFile = argv[i];			
		}
	}
}