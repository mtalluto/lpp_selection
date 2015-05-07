#include <iostream>
#include "../hdr/iterator.h"
#include "../hdr/global.h"

bool iterator_cl::ready()
{
	static bool runOnce = false;
	bool result = false;
	
	if( (!iterating && !runOnce) || (iteratorData.size() > 0) ) {
		result = true;
		runOnce = true;
	}
		
	return result;	
}

iteration_step iterator_cl::update(parameters_cl & params)
{
	iteration_step result;
	
	if(iterating) {
		std::vector<iteration_step>::iterator currentStep = iteratorData.end() - 1;	// grab the last step for convenience
		result = *currentStep;
		while(currentStep->stepData.size() > 0 ) {
			io::input_cl & currentVar = currentStep->stepData.back();	// grab the last variable
			if(currentVar.name() == "predation") {
				// predation values are multipliers to apply to the predation rates
				{ // first nonserotinous rate
					std::vector<double> oldRate;
					std::vector<std::string> newRateStr;
					oldRate.push_back(params.predation_rate_ns());
					std::stringstream ss;
					ss << (oldRate[0] * ::str_convert<double>(currentVar.data()[0]));
					newRateStr.push_back(ss.str());
					io::input_cl nsPredData ("predationRateNser", newRateStr);
					params.set_parameter(nsPredData);
				}
				
				{ // now serotinous
					std::vector<double> oldRate;
					std::vector<std::string> newRateStr;
					oldRate = params.predation_vector_s();
					for(int i = 0; i < oldRate.size(); i++) {
						std::stringstream ss;
						ss << (::str_convert<double>(currentVar.data()[0]) * oldRate[i]);
						newRateStr.push_back(ss.str());					
					}
					io::input_cl sPredData ("predationRateSer", newRateStr);
					params.set_parameter(sPredData);
				}
			} else {		// anything except predation
				params.set_parameter(currentVar);						// shove it into params
			}
			currentStep->stepData.pop_back();						// pop it off
		}
	
		iteratorData.pop_back();
	}
	return result;
}

size_t iterator_cl::size()
{ if(!iterating) return 0; else return iteratorData.size(); }


iterator_cl::iterator_cl(const std::string & iterFile, stats_cl & stats) :
iterating(false)
{
	// open the file and grab the data
	std::vector<io::input_cl> iterationData;
	try{
		io::read_file(iterFile.c_str(), iterationData);
	}
	catch(std::string s) {
		std::cerr << s << std::endl;
		return;
	}
	catch(...) {
		throw;
	}
	
	if(iterationData.size() == 0)
		return;

	iterating = true;

	// step through each iteration parameter and save the sequences to be iterated
	std::vector<std::string> itNames;
	std::vector<std::vector<long double> > itData;
	for(int i = 0; i < iterationData.size(); i++) {
		io::input_cl cPar = iterationData[i];
	
		// grab the iteration steps, then convert to a number, then convert to a sequence and save in itData/itNames
		std::vector<long double> itSteps = ::str_convert<long double>(cPar.data());
		if(itSteps.size() != 3) throw( std::string("Error: iterator names must be followed by an optional -m and exactly 3 numbers") );
		itNames.push_back(cPar.name());
		itData.push_back(::seq<long double>(itSteps[0], itSteps[1], itSteps[2]));
	}

	// step through the saved iteration sequences and create the proper data structure for each step
	std::vector<size_t> index (itData.size(), 0);
	bool done = false;
	while(!done) {
		iteration_step curStep;
		for(int i = 0; i < index.size(); i++) {
			std::stringstream ss;
			ss << itData[i][index[i]];
			std::vector<std::string> strDat;
			strDat.push_back(ss.str());
			curStep.stepData.push_back(io::input_cl(itNames[i], strDat));
		}
		iteratorData.push_back(curStep);
	
		int ind = itData.size() - 1;
		bool success = false;
		while(!success) {
			index[ind]++;
			if(index[ind] < itData[ind].size()) {
				success = true;
			}
			else if (ind >= 0) {
				index[ind] = 0;
				ind--;
			}
			if(ind < 0) {
				success = true;
				done = true;
			}
		}
	}
}