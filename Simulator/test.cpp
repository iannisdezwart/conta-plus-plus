#include <bits/stdc++.h>

#include "../libs/fs.hpp"

#define MULTI_THREADED

#include "simulator.hpp"
#include "random.hpp"

using namespace std;

int main()
{
	SimulationSettings simulation_settings = {
		.POPULATION_SIZE = 1000,
		.NUMBER_OF_COMMUNITIES = 12,
		.HUMAN_MAX_VELOCITY = 5.0,
		.HUMAN_SPREAD_PROBABILITY = 0.5,
		.HUMAN_SPREAD_RANGE = 20,
		.HUMAN_INFECTION_DURATION = 100,
		.HUMAN_TRAVEL_RATIO = 0.01,
		.SOCIAL_DISTANCING_THRESHOLD = 100,
		.SOCIAL_DISTANCING_RELEASE = 60,
		.SOCIAL_DISTANCING_RADIUS = 30
	};

	simulate("output/123.conta", simulation_settings,
		[](int tick_number, Population& population) {
			printf("Rendered tick %d\n", tick_number);
		});
}