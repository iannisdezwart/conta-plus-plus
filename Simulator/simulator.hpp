#ifndef SIMULATOR_HEADER
#define SIMULATOR_HEADER

#include <bits/stdc++.h>

// Globals

const int COMMUNITY_SIZE = 400;

#include "../libs/fs.hpp"
#include "random.hpp"
#include "human.hpp"
#include "population.hpp"
#include "simulation_settings.hpp"

using namespace std;

void simulate(
	string output_file_path,
	SimulationSettings& simulation_settings,
	function<void (int, Population *)> tick_callback
)
{
	fs::File file(output_file_path, "w");
	Population *population = new Population(file, simulation_settings);

	tick_callback(0, population);

	for (int i = 1; i <= 1000; i++) {
		population->tick();
		tick_callback(i, population);
	}

	delete population;
	file.close();
}

#endif
