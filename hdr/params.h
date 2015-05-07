#ifndef PARAMS_H
#define PARAMS_H

#include <vector>
#include "io.h"

class parameters_cl 
{
	// parameters dealing with starting conditions and model settings
	double				startingPopDensity;
	double				startingFreqSerotiny;
	size_t				startingAge;
	size_t				numYears;
	int					endStandAge;
	int					outputFrequency;
	std::vector<size_t>	outputStandAge;
	std::vector<size_t>	outputYears;
	std::vector<bool>	outputSAYear;
	size_t				burnAge;
	size_t				numReps;
	size_t				numGenerations;

	// landscape parameters
	double		maxPopDensity;
	bool		stochasticFire;
	size_t		fireFrequency;
	double		fireProbability;
	double		immigrationRate;		// d_0
	double		postfireDensitySlope;	// d_1
	double		selfThinInt;			// k_0
	double		selfThinSlope;			// k_1
	double		rickerR;				// r
	bool		serotinousInfill;

	// predation parameters
	size_t				predationNumYears;
	size_t				oldConeThreshold;
	double				predationRateSer;
	double				predationRateNser;
	double				medianPredationRateSer;
	double				medianPredationRateNser;
	std::vector<double>	serPredationVector;
	std::vector<double>	medianPredationVector;
	size_t				seedProductionThreshold;	// age at which plants can produce seeds
	std::vector<double>	densToDist;
// 	std::vector<double>	distToSurv;
	double 				distToSurvInt;				// p_0
	double 				distToSurvIntS;				// p_2
	double 				distToSurvSlope;			// p_1
	double 				distToSurvSlopeS;			// p_3

	double				squirrelDensity;
	

	// plant parameters
	double				diMortalityRate;
	double				fireMortalityRate;
	size_t				ageAtAdulthood;
	size_t				serotinyAgeThreshold;

	// private functions
	double	calculate_predation_rate(std::vector<double> & predRate);
	double	age_specific_K_log(const double logAge) const;
	void	set_defaults();
	double	inv_logit(double x);
	void	density_to_predation();
	size_t	max_output_age();
	
	public:
		// methods for reporting model settings and starting conditions
		double	starting_pop_density() const;
		double	starting_freq_ser() const;
		size_t	starting_age() const;
		size_t	num_years() const;
		bool	time_to_output(const size_t year, const size_t age);
		bool	end_stand_age_reached(const size_t age);
		bool	tracking_generations();
		size_t	num_generations();

		// methods for reporting landscape/population-level parameters
		double					max_pop_density() const;
		bool					stochastic_fire() const;
		size_t					fire_frequency() const;
		double					fire_probability() const;
		size_t					burn_age() const;
		size_t					num_reps() const;
		std::vector<double>		postfire_density();
		double					age_specific_K(size_t age) const;
		double					ricker_r() const;
		bool					serotinous_infill() const;
		size_t					seed_production_threshold() const;

		// methods for reporting information about predation rates
		double					predation_rate_ns() const;
		std::vector<double> & 	predation_vector_s();
		bool					predation() const;
		double					predation_ratio(const std::string & s) const;

		// methods for reporting plant-level parameters
		double	di_mortality_rate() const;
		size_t	adult_age() const;					// returns the age at which a plant is considered an adult
		double	fire_mortality_rate() const;
		size_t	serotiny_age_threshold() const;		// age at which a plant expresses serotiny

		// methods for setting up and manipulating parameters
		void	set_parameter(io::input_cl & input);
		parameters_cl(const char * inputFile);
		parameters_cl() { }
};

#endif