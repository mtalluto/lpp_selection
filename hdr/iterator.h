#ifndef ITERATOR_H
#define ITERATOR_H

#include <vector>
#include <string>
#include "params.h"
#include "stats.h"
#include "io.h"

// single iteration step in the iterator -- one tick of the iterator writes all of these to the parameter file
struct iteration_step {
	std::vector<io::input_cl>	stepData;
};

class iterator_cl {
	std::vector<iteration_step>	iteratorData;		// each value in this vector is a single step of the iterator
	bool						iterating;			// true if the iterator was successful in setting itself up, otherwise false
	
	public:
		bool ready();									// returns true if the iterator has values remaining OR if iterating is false and this is the first time ready() has been invoked (to allow a single model run when not iterating)
		iteration_step update(parameters_cl &params);	// grab a set of iteration values and update the parameter object with them
		size_t size();									// returns how many iteration steps remain
		iterator_cl(const std::string & iterFile, stats_cl & stats);
		iterator_cl() {}
};

#endif