#ifndef SIMULATION_SETTINGS_HEADER
#define SIMULATION_SETTINGS_HEADER

#include <bits/stdc++.h>

using namespace std;

struct SimulationSettings {
	int POPULATION_SIZE;
  int NUMBER_OF_COMMUNITIES;
  double HUMAN_MAX_VELOCITY;
  double HUMAN_SPREAD_PROBABILITY;
  int HUMAN_SPREAD_RANGE;
  int HUMAN_INFECTION_DURATION;
};

#endif
