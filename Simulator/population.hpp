#ifndef POPULATION_HEADER
#define POPULATION_HEADER

#include <bits/stdc++.h>
#include "human.hpp"
#include "filebuffer.hpp"

using namespace std;

class Population {
	public:
		FILE *file;

		int tick_count = 0;

		vector<vector<Human>> communities;

		Population(string output_file_name)
		{
			file = fopen(output_file_name.c_str(), "w");

			// Create all community vectors

			for (int i = 0; i < NUMBER_OF_COMMUNITIES; i++) {
				vector<Human> humans_on_this_community;
				communities.push_back(humans_on_this_community);
			}

			int infected_human_index = random_float() * POPULATION_SIZE;

			// Initialise each human

			for (int i = 0; i < POPULATION_SIZE; i++) {
				// Calculate a random starting position

				Vector<2> starting_position({
					random_float() * COMMUNITY_SIZE,
					random_float() * COMMUNITY_SIZE
				});

				// Calculate a random community id

				uint16_t starting_community = random_float() * NUMBER_OF_COMMUNITIES;
				Human human(starting_position, starting_community);

				// Infect this Human if it its index matches

				if (i == infected_human_index) {
					human.infected = true;
				}

				// Add the human to the correct community

				communities[starting_community].push_back(human);
			}

			// Write the file header...

			write_file_header();

			// ...and the starting positions of the humans

			write_tick_to_file();
		}

		void tick()
		{
			tick_count++;

			// Loop over each community

			for (int i = 0; i < NUMBER_OF_COMMUNITIES; i++) {
				vector<Human> *humans = &communities[i];

				// Loop over each Human on the current community

				for (int j = 0; j < humans->size(); j++) {
					// Move the Human

					Human *human = &humans->at(j);

					Vector<2> acceleration = Vector<2>::from_angle(random_float() * 2 * M_PI);
					human->move(acceleration);

					// Infect other Humans if this Human is infected

					if (human->infected) {
						human->ticks_infected++;

						// Infect each other Human that is within the HUMAN_SPREAD_RANGE,
						// with a probability of HUMAN_SPREAD_PROBABILITY

						// Todo: add incubation period here as well

						for (int k = 0; k < humans->size(); k++) {
							if (j != k) {
								Human *other_human = &humans->at(k);
								double distance = human->position.distance(other_human->position);

								if (
									distance <= HUMAN_SPREAD_RANGE
									&& random_float() < HUMAN_SPREAD_PROBABILITY
								) other_human->infected = true;
							}
						}

						// Recover after HUMAN_INFECTION_DURATION ticks

						if (human->ticks_infected >= HUMAN_INFECTION_DURATION) {
							human->infected = false;
							human->recovered = true;
						}
					}
				}
			}

			write_tick_to_file();
		}

		void write_file_header()
		{
			FileBuffer buffer;

			buffer.write("CONTA\n");
			buffer.write(POPULATION_SIZE);
			buffer.write(NUMBER_OF_COMMUNITIES);
			buffer.write('\n');

			fwrite(buffer.data(), 1, buffer.size(), file);
		}

		void write_tick_to_file()
		{
			FileBuffer buffer;

			// Loop over each community

			for (int i = 0; i < NUMBER_OF_COMMUNITIES; i++) {
				vector<Human> *humans = &communities[i];

				// Loop over each Human on the current community

				for (int j = 0; j < humans->size(); j++) {
					// Write the current Human to the FileBuffer

					buffer.write(humans->at(j).as_file_buffer());
				}
			}

			fwrite(buffer.data(), 1, buffer.size(), file);
		}
};

#endif