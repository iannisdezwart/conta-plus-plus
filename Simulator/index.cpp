#include <bits/stdc++.h>

#include "random.hpp"
#include "human.hpp"
#include "population.hpp"

using namespace std;

const int COMMUNITY_SIZE = 400;

int main()
{
	Population population(10, 1, "output/run.conta");

	for (int i = 0; i < 10; i++) {
		printf("Rendering frame %d...\n", i);

		population.tick();
	}
}