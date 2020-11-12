#include <bits/stdc++.h>

#include "simulator.hpp"

using namespace std;

int main()
{
	SimulationSettings simulation_settings = {
		.POPULATION_SIZE = 1000,
		.NUMBER_OF_COMMUNITIES = 12,
		.HUMAN_MAX_VELOCITY = 5.0,
		.HUMAN_SPREAD_PROBABILITY = 0.05,
		.HUMAN_SPREAD_RANGE = 20,
		.HUMAN_INFECTION_DURATION = 100
	};

	simulate(simulation_settings);
}
