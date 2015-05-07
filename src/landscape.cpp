#include <iostream>
#include <iomanip>
#include <numeric>
#include <cassert>
#include <cmath>
#include <sstream>
#include "../hdr/landscape.h"
#include "../hdr/stats.h"

size_t landscape_cl::lastID = 0;

void landscape_cl::start(std::vector<std::string> & popOutput, std::vector<std::string> & fireOutput)
{
	prep_output_header(popOutput, fireOutput);
	size_t year = 0;
	while( !finished(year) ) {
		update(year);
		year++;
		if(params.time_to_output(year, standAge) ) {
			prep_output(year, popOutput, fireOutput);
		}
	}
}

bool landscape_cl::finished(size_t year) {
	bool result = false;
	if(params.end_stand_age_reached(standAge)) {
		if (params.tracking_generations())
			result = numGenerations >= params.num_generations();
		else
			result = year >= params.num_years();
	}
	
	return result;
}


void landscape_cl::prep_output(const size_t year, std::vector<std::string> & pop, std::vector<std::string> & fire) 
{
	std::stringstream result;
//	result << std::setprecision(5) << std::fixed;
	result << ID << "," << year << "," << standAge << "," << popDensity.back() << "," << populationDensity.nser() << "," << populationDensity.ser() << "," << freqSerotiny.back() << "\n";
	pop.push_back(result.str());
	
	for(int i = 0; i < fireHistory.size(); i++) {
		result.str(std::string());	// empty the stream
		result << ID << "," << fireHistory[i] << "\n";
		fire.push_back(result.str());
	}
	fireHistory.clear();
}

void landscape_cl::prep_output_header(std::vector<std::string> & pop, std::vector<std::string> & fire) 
{
	pop.push_back("id,year,standAge,popSizeTotal,popSizeNS,popSizeS,freqSerotiny\n");
	fire.push_back("id,year\n");
}

// the update() function represents the annual cycle of the model
void landscape_cl::update(const size_t & year)
{
	// check for fire
	bool burning = is_burning(year);
	if(burning) {
		fireHistory.push_back(year);
		numGenerations++;
	}
	
	// DI mortality (adults only)
	{
		ser_cl diDead ( populationDensity.ser() * params.di_mortality_rate(), populationDensity.nser() * params.di_mortality_rate() );
		populationDensity.subtract(diDead);
	}

	// mortality from fire and postfire regeneration
	if(burning)
		postfire_regeneration_mortality();	// in fire years, we use postfire regeneration
	else
		density_dependent_growth();			// in nonfire years, we use the ricker model
		
	// age the forest
	if(burning)
		standAge = 0;
	else
		standAge++;
		
	// calculate statistics
	calculate_stats();
	
}

void landscape_cl::density_dependent_growth()
{
	// calculate K for the present stand age
	double currentK = params.age_specific_K(standAge);

	// current population density
	double curDensity = populationDensity.sum();
		
	double newDensity;
	// if the population is extinct, bring in plants from the outside
	if(curDensity == 0)
		newDensity = std::exp(params.postfire_density()[0]); // use the intercept from the fire curve ~ 600 individuals/year
	else
		// calculate the target population size using the Ricker model
		newDensity = curDensity * std::exp( params.ricker_r() * (1 - (curDensity / currentK)));
		
	if(newDensity < curDensity) {
		// population is shrinking; kill them off in proportion to their numbers
		double serRatio = populationDensity.ser() / curDensity;
		populationDensity.ser() = newDensity * serRatio;
		populationDensity.nser() = newDensity - populationDensity.ser();
	}
	else if(newDensity > curDensity) {
		// population is growing -- add nonserotinous individuals
		double newInd = newDensity - curDensity;

		// SEED PREDATION -- penalize nonserotinous regeneration using the ns predation rate
		if(params.predation() )
			newInd = newInd * params.predation_ratio("ns");				// this is 1/ (the ratio of the predation rate to the median rate) -- so 2x as much predation results in 1/2 as many seeds
		
		if(standAge >= params.serotiny_age_threshold() || !params.serotinous_infill()) {
			// for old stands or model runs where we aren't doing infilling with serotinous trees, all new trees are nonserotinous
			populationDensity.nser() += newInd;
		}
		else if(standAge < params.seed_production_threshold()) {
			// for very young stands, all input is from immigration
			// use the intercept from the postfire recovery curve as nonserotinous immigrants
			double newNSInd = std::exp(params.postfire_density()[0]);
			if(newNSInd > newInd) newNSInd = newInd;
			populationDensity.nser() += newNSInd;
		}
		else {
			// for intermediate stands, infilling is accomplished by both immigration and local seed production in proportion to the frequency of serotiny
			double newSInd = 0, newNSInd = 0;
			double freqSer = populationDensity.ser() / populationDensity.sum();
			
			// figure out what % of the maximum stand density at this age we are at; this is the proportion of
			// seeds that come from local sources vs. immigrants
			double popRatio = populationDensity.sum() / currentK;
			double newImmigrants = (1-popRatio) * newInd;
			double newLocal = popRatio * newInd;
			
			// use the intercept from the postfire recovery curve as nonserotinous immigrants
			newNSInd = std::exp(params.postfire_density()[0]);
			if(newNSInd > newImmigrants)
				newNSInd = newImmigrants;

			// the remainder are in proportion to frequency
			newSInd += freqSer * newLocal;
			newNSInd += (1-freqSer) * newLocal;
			
			populationDensity.nser() += newNSInd;
			populationDensity.ser() += newSInd;
		}
	}
}

void landscape_cl::postfire_regeneration_mortality()
{
	// calculate frequency of serotiny
	double freqSer = populationDensity.ser() / populationDensity.sum();
	
	// alter frequency of serotiny using the predation rate
	if(params.predation()) {
		freqSer = freqSer * params.predation_ratio("s");
		if(freqSer > 1) freqSer = 1;		
	}
	
	// kill individuals
	populationDensity.ser() *= (1 - params.fire_mortality_rate()); 
	populationDensity.nser() *= (1 - params.fire_mortality_rate()); 

	// postfire recovery
	// calculate seedling density
	double seedDens = std::exp(params.postfire_density()[0] + params.postfire_density()[1] * freqSer);
	
	// assume the intercept represents nonserotinous immigrants and all others are serotinous
	populationDensity.nser() += std::exp(params.postfire_density()[0]);
	populationDensity.ser() += (seedDens - populationDensity.nser());
}

bool landscape_cl::is_burning(const size_t & year)
{
	bool result = false;
	if(standAge >= params.burn_age() ) {
		if (params.stochastic_fire()) 
			result = (stats->runif() < params.fire_probability());
		else
			result = standAge >= params.fire_frequency();
//			old method used the year; changed for deterministic models so that  there is no issue of starting conditions
//			(year != 0 ) && (( year % params.fire_frequency() ) == 0);
	}
	return result;
}

void landscape_cl::make_default_population()
{
	// default population is a population of full adults at maximum density
	standAge = params.starting_age();
	populationDensity = ser_cl (params.starting_freq_ser() * params.starting_pop_density(), params.starting_pop_density() - (params.starting_freq_ser() * params.starting_pop_density()));
}

void landscape_cl::calculate_stats()
{
	popDensity.push_back(populationDensity.sum());
	freqSerotiny.push_back( populationDensity.ser() / populationDensity.sum());
}

/*-------------------|
|    CONSTRUCTOR     |
|-------------------*/

landscape_cl::landscape_cl(parameters_cl _params, stats_cl * _stats) :
params(_params), ID(lastID++), stats(_stats), numGenerations(0)
{
	make_default_population();
}