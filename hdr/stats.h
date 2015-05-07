#ifndef STATS_H
#define STATS_H

#include <vector>
#include <gsl/gsl_rng.h>	

class stats_cl {
	private:
		gsl_rng * RNG;
		size_t	* refCount;			// keeps track of how many copies have been made
	
	public:
		// random numbers
		double runif(double a = 0, double b = 1);
		unsigned int rbinom(double p, unsigned int n);
		double rnorm(double mu, double s);
		
		std::vector<size_t> shuffle(std::vector<size_t> &input);
		
		stats_cl & operator=(const stats_cl & stRef);
		stats_cl(const stats_cl & stRef); 
		
		stats_cl(unsigned long int seed);
		stats_cl();
		~stats_cl();
};

#endif