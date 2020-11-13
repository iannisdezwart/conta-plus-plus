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

void simulate(string output_file_path, SimulationSettings& simulation_settings)
{
	fs::File file(output_file_path, "w");
	Population population(file, simulation_settings);

	for (int i = 0; i < 999; i++) {
		printf("Rendering frame %d...\n", i);

		population.tick();
	}

	file.close();
}

#endif
