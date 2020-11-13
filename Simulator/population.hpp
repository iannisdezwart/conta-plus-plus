#ifndef POPULATION_HEADER
#define POPULATION_HEADER

#include <bits/stdc++.h>

#include "../libs/fs.hpp"
#include "human.hpp"
#include "filebuffer.hpp"
#include "simulation_settings.hpp"

using namespace std;

class Population {
	public:
		fs::File& file;
		SimulationSettings& settings;

		int tick_count = 0;

		vector<vector<Human>> communities;

		Population(fs::File& output_file, SimulationSettings& simulation_settings)
			: settings(simulation_settings), file(output_file)
		{
			// Create all community vectors

			for (int i = 0; i < settings.NUMBER_OF_COMMUNITIES; i++) {
				vector<Human> humans_on_this_community;
				communities.push_back(humans_on_this_community);
			}

			int infected_human_index = random_float() * settings.POPULATION_SIZE;

			// Initialise each human

			for (int i = 0; i < settings.POPULATION_SIZE; i++) {
				// Calculate a random starting position

				Vector<2> starting_position({
					random_float() * COMMUNITY_SIZE,
					random_float() * COMMUNITY_SIZE
				});

				// Calculate a random community id

				uint16_t starting_community = random_float() * settings.NUMBER_OF_COMMUNITIES;
				Human human(starting_position, starting_community, settings);

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

			// We must first run the tick, then infect the newly infected Humans
			// Otherwise, Humans could potentially reinfect each other

			vector<Human *> new_infections;

			// Loop over each community

			for (int i = 0; i < settings.NUMBER_OF_COMMUNITIES; i++) {
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
									other_human->susceptible()
									&& distance <= settings.HUMAN_SPREAD_RANGE
									&& random_float() < settings.HUMAN_SPREAD_PROBABILITY
								) {
									new_infections.push_back(other_human);
								}
							}
						}

						// Recover after HUMAN_INFECTION_DURATION ticks

						if (human->ticks_infected >= settings.HUMAN_INFECTION_DURATION) {
							human->infected = false;
							human->recovered = true;
						}
					}
				}
			}

			// Actually infect the new infected people

			for (int i = 0; i < new_infections.size(); i++) {
				new_infections[i]->infected = true;
			}

			write_tick_to_file();
		}

		void write_file_header()
		{
			FileBuffer buffer;

			buffer.write("CONTA\n");
			buffer.write(settings.POPULATION_SIZE);
			buffer.write(settings.NUMBER_OF_COMMUNITIES);
			buffer.write('\n');

			file.write(buffer.data(), buffer.size());
		}

		void write_tick_to_file()
		{
			FileBuffer buffer;

			// Loop over each community

			for (int i = 0; i < settings.NUMBER_OF_COMMUNITIES; i++) {
				vector<Human> *humans = &communities[i];

				// Loop over each Human on the current community

				for (int j = 0; j < humans->size(); j++) {
					// Write the current Human to the FileBuffer

					buffer.write(humans->at(j).as_file_buffer());
				}
			}

			file.write(buffer.data(), buffer.size());
		}
};

#endif
