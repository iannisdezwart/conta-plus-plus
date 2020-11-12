#ifndef SIMULATOR_HEADER
#define SIMULATOR_HEADER

#include <bits/stdc++.h>

// Globals

const int COMMUNITY_SIZE = 400;

#include "random.hpp"
#include "human.hpp"
#include "population.hpp"
#include "simulation_settings.hpp"

using namespace std;

void simulate(SimulationSettings& simulation_settings)
{
	Population population("output/0.conta", simulation_settings);

	for (int i = 0; i < 1000; i++) {
		printf("Rendering frame %d...\n", i);

		population.tick();
	}
}

#endif
