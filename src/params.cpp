#include <iostream>
#include <cmath>
#include <cassert>
#include "../hdr/params.h"
#include "../hdr/global.h"

bool parameters_cl::predation() const
{
	if(predationRateNser == -1 || predationRateSer == -1)
		return false;
	else return true;
}

double parameters_cl::predation_ratio(const std::string & s) const
{
	double rate, median;
	if( s == "s" ) {
		rate = predationRateSer;
		median = medianPredationRateSer;
	}
	else if( s == "ns" ) {
		rate = predationRateNser;
		median = medianPredationRateNser;
	}
	else throw(std::string("unknown predation mode" + s));
	
	double remainingSeeds = 1 - rate;
	double remainingMedian = 1 - median;
	double ratio = remainingSeeds/remainingMedian;
	
	return ratio;
}

void parameters_cl::density_to_predation()
{
	/*
		dens to dist can be done analytically as well, if desired
		assuming middens are placed on a regular grid and are d meters apart, with density D in ind/ha
		the average distance can be approximated as half the radius of a circle with diameter d
		and is equal to sqrt(2500/D) (the 2500 is converting hectares to meters)		
	*/
	
	// convert density to distance
	if(squirrelDensity < 0)
		throw("Squirrel density must be greater than 0");
	else if(squirrelDensity == 0) {
		predationRateNser = 0;
		predationRateSer = 0;
	} else {
		double distance = densToDist[0] + densToDist[1] * std::log(squirrelDensity);
		// convert distance to predation rates
		serPredationVector = std::vector<double> (2,0);
		predationRateNser = 1 - inv_logit(distToSurvInt + distToSurvSlope * distance);
// 		serPredationVector[0] = 1 - inv_logit(distToSurv[0] + distToSurv[1] + (distToSurv[3] + distToSurv[4]) * distance);
// 		serPredationVector[1] = 1 - inv_logit(distToSurv[0] + distToSurv[2] + (distToSurv[3] + distToSurv[5]) * distance);
// 		predationRateSer = calculate_predation_rate(serPredationVector);
		predationRateSer = 1 - inv_logit(distToSurvInt + distToSurvIntS + (distToSurvSlope+distToSurvSlopeS) * distance);
	}
	
}

double parameters_cl::inv_logit(double x)
{ return std::exp(x) / (1 + std::exp(x)); }

double parameters_cl::predation_rate_ns() const
{ return predationRateNser; }
 
std::vector<double> & parameters_cl::predation_vector_s() 
{ return serPredationVector; }


double parameters_cl::max_pop_density() const
{ return maxPopDensity; }

size_t parameters_cl::num_years() const
{ return numYears; }

double parameters_cl::di_mortality_rate() const
{ return diMortalityRate; }

size_t parameters_cl::adult_age() const
{ return ageAtAdulthood; }

size_t parameters_cl::num_reps() const
{ return numReps; }

double parameters_cl::starting_pop_density() const
{ 
	if(startingPopDensity == 0) 
		return age_specific_K(starting_age()); 
	else 
		return startingPopDensity;
}

bool parameters_cl::tracking_generations()
{ return (num_generations() > 0); }

size_t parameters_cl::num_generations()
{ return numGenerations; }

double parameters_cl::starting_freq_ser() const
{ return startingFreqSerotiny; }

double parameters_cl::fire_mortality_rate() const
{ return fireMortalityRate; }

double parameters_cl::ricker_r() const
{ return rickerR; }

bool parameters_cl::stochastic_fire() const
{ return stochasticFire; }

size_t parameters_cl::fire_frequency() const
{ return fireFrequency;}

double parameters_cl::fire_probability() const
{ return fireProbability; }

size_t parameters_cl::starting_age() const
{ return startingAge; }

size_t parameters_cl::seed_production_threshold() const
{ return seedProductionThreshold; }

bool parameters_cl::serotinous_infill() const
{ return serotinousInfill; }

size_t parameters_cl::burn_age() const
{ return burnAge; }

size_t parameters_cl::serotiny_age_threshold() const
{ return serotinyAgeThreshold; }

std::vector<double> parameters_cl::postfire_density()
{ 
	std::vector<double> result;
	result.push_back(immigrationRate);
	result.push_back(postfireDensitySlope);
	return result;
}

bool parameters_cl::time_to_output(const size_t year, const size_t age)
{ 
	bool result = false;
	
	// outputYears says to output ONLY during specific years
	if(outputYears.size() > 0) {
		if(outputStandAge.size() > 0) {
			// if we are doing specific stand ages, the behavior is to return true iff: stand age == age, we are >= than a given year, and we haven't already output for that year
			// see if the current age is on the list
			bool sa = std::find(outputStandAge.begin(), outputStandAge.end(), age) != outputStandAge.end();
			if(sa) {
				for(int i = 0; i < outputYears.size(); i++) {
					if(year >= outputYears[i] && !(outputSAYear[i])) {
						result = true;
						outputSAYear[i] = true;
					}
				}
			}
		}
		else {
			// if we aren't doing specific stand ages, search the vector for this specific year and report whether it is found
			result = std::find(outputYears.begin(), outputYears.end(), year) != outputYears.end();
		}
	}
	else if(outputStandAge.size() > 0) {
		// covers the case where we are doing specific stand ages, but not specific years
		result = std::find(outputStandAge.begin(), outputStandAge.end(), age) != outputStandAge.end();
	}
	if(outputFrequency != -1) {
		// covers the case where we are outputting with a certain regular frequency
		result = result || (year % outputFrequency) == 0;
	}
	
	return result;
}

bool parameters_cl::end_stand_age_reached( const size_t age )
{
	if( endStandAge == -1 ) {
		return (age >= max_output_age());
	} else
		return (age == endStandAge);
}

size_t parameters_cl::max_output_age() {
	size_t max = 0;
	for(int i = 0; i < outputStandAge.size(); i++)
		if(max < outputStandAge.at(i)) max = outputStandAge.at(i);
	return max;
}

double parameters_cl::age_specific_K_log(const double logAge) const
{
	// returns the log(number of trees) that can be supported per hectare of a given age
	return double(selfThinInt + (selfThinSlope*logAge));
}

double parameters_cl::age_specific_K(size_t age) const
{
	if(age == 0) age += 1;				// we add one to account for the fact that the first age I use is 0; so log(1) = 0 is the intercept of the curve on the log scale
	double result = std::exp(age_specific_K_log(std::log(age)));
	if(result < maxPopDensity)
		result = maxPopDensity;
		
	return result;
}


void parameters_cl::set_defaults()
{
	numReps = 1;
	maxPopDensity = 1200;
	numYears = 1000;
	numGenerations = 0;
	diMortalityRate = 0.005;
	ageAtAdulthood = 200;
	startingAge = ageAtAdulthood;
	startingPopDensity = 0;
	startingFreqSerotiny = 0.25;
	fireMortalityRate = 1;
	fireFrequency = 250;
	fireProbability = 0;
	stochasticFire = false;
	outputFrequency = 10;
	serotinyAgeThreshold = 30;
	burnAge = 	100;
	rickerR = 0.05;
	serotinousInfill = true;
	predationRateSer = -1;
	predationRateNser = medianPredationRateNser = 0.16433759;
	medianPredationVector = std::vector<double> (2,0);
		medianPredationVector[0] = 0.20043820;
		medianPredationVector[1] = 0.02354945;
	serPredationVector = medianPredationVector;
	predationNumYears = 6;	// i.e., first-year cones, plus 5 years of brown cones
	oldConeThreshold = 7;
	medianPredationRateSer = calculate_predation_rate(medianPredationVector);
	seedProductionThreshold = 10;
	endStandAge = -1;
	densToDist = std::vector<double> (2,0);
		densToDist[0] = 58.58318;
		densToDist[1] = -21.21574;
	distToSurvInt = -0.5743626;
	distToSurvIntS = -1.7654844;
	distToSurvSlope = 0.0334646;
	distToSurvSlopeS = 0.0157716;
	squirrelDensity = 0;
	postfireDensitySlope = 8.869651;
	immigrationRate = 6.556999;
	selfThinInt = 14.63611;
	selfThinSlope = -1.39351;
}

void parameters_cl::set_parameter(io::input_cl &input)
{
	std::string name = ::make_lowercase(input.name());
	std::vector<std::string> val = input.data();
	
	
	if(name == "startingpopdensity")			startingPopDensity = ::str_convert<size_t>(val.at(0));
	else if(name == "maxpopdensity")			maxPopDensity = ::str_convert<double>(val.at(0));
	else if(name == "numyears")					numYears = ::str_convert<size_t>(val.at(0));
	else if(name == "numgenerations")			numGenerations = ::str_convert<size_t>(val.at(0));
	else if(name == "dimortalityrate")			diMortalityRate = ::str_convert<double>(val.at(0));
	else if(name == "ageatadulthood")			ageAtAdulthood = ::str_convert<size_t>(val.at(0));
	else if(name == "immigrationrate" || name == "d_0")			immigrationRate = ::str_convert<double>(val.at(0));
	else if(name == "postfiredensityslope" || name == "d_1")	postfireDensitySlope = ::str_convert<double>(val.at(0));
	else if(name == "startingage")				startingAge = ::str_convert<size_t>(val.at(0));
	else if(name == "startingfreqserotiny")		startingFreqSerotiny = ::str_convert<double>(val.at(0));
	else if(name == "firemortalityrate")		fireMortalityRate = ::str_convert<double>(val.at(0));
	else if(name == "fireprobability")			{ fireProbability = ::str_convert<double>(val.at(0)); stochasticFire = true; }
	else if(name == "firerotation")				{ fireProbability = 1/(::str_convert<double>(val.at(0))); stochasticFire = true; }
	else if(name == "firefrequency")			{ fireFrequency = ::str_convert<size_t>(val.at(0)); stochasticFire = false; }
	else if(name == "burnage")					burnAge = ::str_convert<size_t>(val.at(0));
	else if(name == "endstandage")				endStandAge = ::str_convert<int>(val.at(0));
	else if(name == "outputfrequency")			outputFrequency = ::str_convert<int>(val.at(0));
	else if(name == "outputstandage")			outputStandAge = ::str_convert<size_t>(val);
	else if(name == "outputyears")				{ outputYears = ::str_convert<size_t>(val); outputSAYear = std::vector<bool> (outputYears.size(), false); }
	else if(name == "rickerr" || name == "r")	rickerR = ::str_convert<double>(val.at(0));
	else if(name == "selfthinint" || name == "k_0")		selfThinInt = ::str_convert<double>(val[0]);
	else if(name == "selfthinslope" || name == "k_1")	selfThinSlope = ::str_convert<double>(val[0]);
	else if(name == "predationrateser")			{ serPredationVector = ::str_convert<double>(val); predationRateSer = calculate_predation_rate(serPredationVector); }
	else if(name == "predationratenser")		predationRateNser = ::str_convert<double>(val.at(0));
	else if(name == "predationnumyears")		{ predationNumYears = ::str_convert<size_t>(val.at(0)); predationRateSer = calculate_predation_rate(serPredationVector); medianPredationRateSer = calculate_predation_rate(medianPredationVector);}
	else if(name == "oldconethreshold")			{ oldConeThreshold = ::str_convert<size_t>(val.at(0)); predationRateSer = calculate_predation_rate(serPredationVector); medianPredationRateSer = calculate_predation_rate(medianPredationVector);}
	else if(name == "serotinyagethreshold")		serotinyAgeThreshold = ::str_convert<size_t>(val.at(0));
	else if(name == "serotinousinfill")			serotinousInfill = ::str_convert<bool>(val.at(0));
	else if(name == "denstodist")				{ densToDist = ::str_convert<double>(val); density_to_predation(); }
	else if(name == "disttosurvint" || name == "p_0")		distToSurvInt = ::str_convert<double>(val[0]);
	else if(name == "disttosurvints" || name == "p_2")		distToSurvIntS = ::str_convert<double>(val[0]);
	else if(name == "disttosurvslope" || name == "p_1")		distToSurvSlope = ::str_convert<double>(val[0]);
	else if(name == "disttosurvslopes" || name == "p_3")	distToSurvSlopeS = ::str_convert<double>(val[0]);
	else if(name == "squirreldensity")			{ squirrelDensity = ::str_convert<double>(val.at(0)); density_to_predation(); }
	else if(name == "seedproductionthreshold")	seedProductionThreshold = ::str_convert<size_t>(val.at(0));
	else if(name == "numreps" || name == "reps")	numReps = ::str_convert<size_t>(val.at(0));
	else throw(std::string("Unknown input parameter: " + name + "\n"));
}

double parameters_cl::calculate_predation_rate(std::vector<double> & predVector)
{
	double predRate;
	if(predationNumYears == 1) {
		assert(predVector.size() > 0);
		predRate = predVector[0];
	} else if(predationNumYears < oldConeThreshold) {
		assert(predVector.size() > 1);
		predRate = 1 - ((1-predVector[0]) * std::pow((1-predVector[1]), double(predationNumYears - 1)));
	} else {
		assert(predVector.size() > 2);
		predRate = 1 - ((1-predVector[0]) * std::pow((1-predVector[1]), double(oldConeThreshold - 2)) * std::pow((1-predVector[2]), double(predationNumYears - (oldConeThreshold - 1))));
	}
	
	return predRate;
}

/*-------------------|
|    CONSTRUCTOR     |
|-------------------*/

parameters_cl::parameters_cl( const char * inputFile )
{
	set_defaults();
	
	// read input data from disk
	std::vector<io::input_cl> inputData;
	try{
		io::read_file(inputFile, inputData);
		// process the input data
		while(!inputData.empty()) {
			set_parameter(inputData.back());
			inputData.pop_back();
		}
	}
	catch (std::string s) {
		std::cerr << s << std::endl;
	}
	catch (...) {
		throw;
	}
}