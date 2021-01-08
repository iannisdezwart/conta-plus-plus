#ifndef SIMULATOR_HEADER
#define SIMULATOR_HEADER

#include <bits/stdc++.h>

// Globals

const int COMMUNITY_SIZE = 512;

#include "../libs/fs.hpp"
#include "random.hpp"
#include "human.hpp"
#include "population.hpp"
#include "simulation_settings.hpp"

using namespace std;

void simulate(
	string output_file_path,
	SimulationSettings& simulation_settings,
	function<void (int, Population&)> tick_callback = NULL
)
{
	fs::File file(output_file_path, "w");
	Population population(file, simulation_settings);

	if (tick_callback != NULL) tick_callback(0, population);

	int i = 0;

	while (population.infected_count) {
		population.tick();
		if (tick_callback != NULL) tick_callback(i++, population);
		if (i >= simulation_settings.MAX_TICKS) break;
	}

	file.close();
}

#endif
