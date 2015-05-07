#ifndef LANDSCAPE_H
#define LANDSCAPE_H

//	INCLUDES
#include <vector>
#include "params.h"


//	FORWARD DECLARATIONS

class ser_cl;				//	provides a simple class for keeping track of things that could be either serotinous or nonserotinous; has ser and nser members for each
class landscape_cl;			// keeps track of the overall landscape
class stats_cl;

//	CLASSES & STRUCTS

class ser_cl {
		double Ser, Nser;
		
	public:
		double & ser() { return Ser; }
		double & nser() { return Nser; }		
		void add(ser_cl a) { Ser += a.Ser; Nser += a.Nser; }
		void subtract(ser_cl a) {
			if(a.Ser > Ser)		Ser = 0; 
			else 				Ser -= a.Ser; 
			if(a.Nser > Nser) 	Nser = 0; 
			else 				Nser -= a.Nser; }
		double sum() { return Ser+Nser; }
		void clear() { Ser = 0; Nser = 0; }

		ser_cl (double s, double ns) : Ser(s), Nser(ns) {}
		ser_cl () {}
};


class landscape_cl
{
	stats_cl *		stats;
	parameters_cl	params;				// parameters governing landscape behavior
	
	static size_t	lastID;
	size_t			ID;
	size_t			standAge;			// how many years since the forest burned?
	size_t			numGenerations;		// number of generations completed by this stand
	ser_cl			populationDensity;

	// population statistics
	std::vector<size_t>		popDensity;
	std::vector<double>		freqSerotiny;
	std::vector<size_t>		fireHistory;
	
	bool	finished(size_t year);
	void	make_default_population();
	void	update(const size_t & year);
	void	self_thin();
	void	calculate_stats();
	bool	is_burning(const size_t & year);
	void	postfire_regeneration_mortality();
	void	prep_output_header(std::vector<std::string> & pop, std::vector<std::string> & fire);
	void	prep_output(const size_t year, std::vector<std::string> & pop, std::vector<std::string> & fire);
	void	density_dependent_growth();
	
	public:
		void start(std::vector<std::string> & popOutput, std::vector<std::string> & fireOutput);
		
		landscape_cl(parameters_cl _params, stats_cl * _stats);
		landscape_cl() { }

};

#endif