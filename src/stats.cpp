#include "../hdr/stats.h"
#include <gsl/gsl_randist.h>
#include <gsl/gsl_statistics_double.h>
#include <ctime>
#include <iostream>

using std::vector;

	/*-----------------------
	|						|
	|	  stats_cl			|
	|						|
	-----------------------*/	

double stats_cl::runif(double a, double b)
{
	double num;
	if(a > b) {
		int c = b;
		b = a;
		a = c;
	}

	if(a == 0 && b == 1)
		num = gsl_rng_uniform(RNG);
	else
		num = gsl_ran_flat(RNG, a, b);
		
	return num;
}

unsigned int stats_cl::rbinom(double p, unsigned int n)
{ return gsl_ran_binomial(RNG, p, n); }

double stats_cl::rnorm(double mu, double s)
{ return (mu + gsl_ran_gaussian(RNG, s)); }


vector<size_t> stats_cl::shuffle(vector<size_t> &input)
{
	// takes an input vector, shuffles its elements, and returns the resulting vector

	// create an array of indices
 	size_t indices[input.size()];
 	for(int i = 0; i < input.size(); i++)
 		indices[i] = i;
 	
 	// shuffle the array
 	gsl_ran_shuffle(RNG, indices, input.size(), sizeof(size_t));
	
	// store results
	vector<size_t> result;
	for(int i = 0; i < input.size(); i++)
		result.push_back( input.at(indices[i]) );

	return result;
}

	/*-----------------------
	|						|
	|	copy constructors	|
	|						|
	-----------------------*/	

stats_cl & stats_cl::operator=(const stats_cl & stRef) 
{
	if(this == &stRef)
		return *this;
		
	RNG = stRef.RNG;
	refCount = stRef.refCount;
	(*refCount)++;
	return *this;
}

stats_cl::stats_cl(const stats_cl & stRef)
{
	RNG = stRef.RNG;
	refCount = stRef.refCount;
	(*refCount)++;
}


	/*-----------------------
	|						|
	|	  constructors		|
	|						|
	-----------------------*/	
#include <iostream>
stats_cl::stats_cl()
{
	// get an initial seed from the clock
	unsigned long int seed = (int) std::time(NULL);
	
	// set a new RNG using that seed, then get a new seed
	RNG = gsl_rng_alloc (gsl_rng_mt19937);
	gsl_rng_set(RNG, seed);
	seed = gsl_rng_get(RNG);
	
	// reset the RNG
	gsl_rng_set(RNG, seed);
	refCount = new size_t;
	*refCount = 1;
}

stats_cl::stats_cl(unsigned long int seed)
{
	RNG = gsl_rng_alloc (gsl_rng_mt19937);
	gsl_rng_set(RNG, seed);
	refCount = new size_t;
	*refCount = 1;
}
	/*-----------------------
	|						|
	|	  destructor		|
	|						|
	-----------------------*/	

stats_cl::~stats_cl()
{
	(*refCount)--;
	if(*refCount == 0) {
		delete refCount;
		gsl_rng_free(RNG);
	}
}