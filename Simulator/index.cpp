#include <bits/stdc++.h>

// Globals

const int COMMUNITY_SIZE = 400;

// Population settings

int POPULATION_SIZE;
int NUMBER_OF_COMMUNITIES;

// Human settings

double HUMAN_MAX_VELOCITY;

double HUMAN_SPREAD_PROBABILITY;
int HUMAN_SPREAD_RANGE;

int HUMAN_INFECTION_DURATION;

#include "random.hpp"
#include "human.hpp"
#include "population.hpp"

using namespace std;

int main()
{
	// Collect settings

	cout << "POPULATION_SIZE >> ";
	cin >> POPULATION_SIZE;

	cout << "NUMBER_OF_COMMUNITIES >> ";
	cin >> NUMBER_OF_COMMUNITIES;

	cout << "HUMAN_MAX_VELOCITY >> ";
	cin >> HUMAN_MAX_VELOCITY;

	cout << "HUMAN_SPREAD_PROBABILITY >> ";
	cin >> HUMAN_SPREAD_PROBABILITY;

	cout << "HUMAN_SPREAD_RANGE >> ";
	cin >> HUMAN_SPREAD_RANGE;

	cout << "HUMAN_INFECTION_DURATION >> ";
	cin >> HUMAN_INFECTION_DURATION;

	Population population("output/0.conta");

	for (int i = 0; i < 1000; i++) {
		printf("Rendering frame %d...\n", i);

		population.tick();
	}
}