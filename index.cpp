#include <bits/stdc++.h>

#include "random.hpp"
#include "human.hpp"
#include "population.hpp"

using namespace std;

const int COMMUNITY_SIZE = 400;

int main()
{
	Population population(10000, 1, "output/run.conta");

	for (int i = 0; i < 1000; i++) {
		printf("Rendering frame %d...\n", i);

		population.tick();
	}
}