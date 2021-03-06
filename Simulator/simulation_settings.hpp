#ifndef SIMULATION_SETTINGS_HEADER
#define SIMULATION_SETTINGS_HEADER

#include <bits/stdc++.h>

using namespace std;

struct SimulationSettings {
	int POPULATION_SIZE;
	int MAX_TICKS;
  int NUMBER_OF_COMMUNITIES;
  double HUMAN_MAX_VELOCITY;
  double HUMAN_SPREAD_PROBABILITY;
  int HUMAN_SPREAD_RANGE;
	int HUMAN_INCUBATION_PERIOD;
  int HUMAN_INFECTION_DURATION;
  int HUMAN_PROTECTION_DURATION;
	double HUMAN_TRAVEL_RATIO;
	int SOCIAL_DISTANCING_THRESHOLD;
	int SOCIAL_DISTANCING_RELEASE;
	int SOCIAL_DISTANCING_RADIUS;
	double SOCIAL_DISTANCING_RATIO;
};

#endif
