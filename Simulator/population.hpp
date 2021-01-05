#ifndef POPULATION_HEADER
#define POPULATION_HEADER

#include <bits/stdc++.h>
#include <thread>

#include "../libs/fs.hpp"
#include "random.hpp"
#include "human.hpp"
#include "filebuffer.hpp"
#include "simulation_settings.hpp"
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

using namespace std;
using ThreadPool = boost::asio::thread_pool;

const int NUM_OF_THREADS = thread::hardware_concurrency();

class Population {
	public:
		fs::File& file;
		SimulationSettings& settings;

		int tick_count = 0;

		vector<vector<Human>> communities;

		RandomIntGenerator travel_rng;
		RandomIntGenerator community_rng;

		ThreadPool thread_pool;

		Population(fs::File& output_file, SimulationSettings& simulation_settings)
			: settings(simulation_settings),
				file(output_file),
				travel_rng(1, settings.POPULATION_SIZE - 1),
				community_rng(0, settings.NUMBER_OF_COMMUNITIES - 1),
				thread_pool(NUM_OF_THREADS)
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

				uint8_t starting_community = community_rng.generate();

				// Create the Human

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

		void loop_over_community(int com_i, function<void (Human *, int)> callback)
		{
			Population *pop = this;

			for (int thr_i = 0; thr_i < NUM_OF_THREADS; thr_i++) {
				boost::asio::post(thread_pool, [com_i, thr_i, pop, callback]() {
					int i = thr_i;
					int com_size = pop->communities[com_i].size();

					while (i < com_size) {
						callback(&pop->communities[com_i][i], i);
						i += NUM_OF_THREADS;
					}
				});
			}
		}

		void loop_over_population(function<void (Human *, int)> callback)
		{
			for (int i = 0; i < settings.NUMBER_OF_COMMUNITIES; i++) {
				loop_over_community(i, callback);
			}
		}

		~Population()
		{
			// Join the threads

			thread_pool.join();
		}

		void move_human(uint8_t old_community_id, int index, uint8_t new_community_id)
		{
			Human& human = communities[old_community_id][index];

			// Set human community id

			human.community_id = new_community_id;

			// Delete human from the old community

			vector<Human>& community = communities[old_community_id];
			int community_size = community.size();

			if (index != community_size - 1) {
				// Swap the human with the last human in its community

				community[index] = community[community_size - 1];
			}

			community.pop_back();

			// Insert the human into the new community

			communities[new_community_id].push_back(human);
		}

		void tick()
		{
			tick_count++;

			// We must first run the tick, then infect the newly infected Humans
			// Otherwise, Humans could already infect others in their 0-th infection tick

			vector<Human *> new_infections;

			// Loop over each community

			for (int i = 0; i < settings.NUMBER_OF_COMMUNITIES; i++) {
				vector<Human>& humans = communities[i];

				// Loop over each Human on the current community

				loop_over_community(i, [&](Human *human, int j) {
					// Move the Human

					Vector<2> acceleration = Vector<2>::from_angle(random_float() * 2 * M_PI);
					human->move(acceleration);

					// Infect other Humans if this Human is infected

					if (human->infected) {
						human->ticks_infected++;

						// Infect each other Human that is within the HUMAN_SPREAD_RANGE,
						// with a probability of HUMAN_SPREAD_PROBABILITY

						// Todo: add incubation period here as well

						for (int k = 0; k < humans.size(); k++) {
							if (j != k) {
								Human& other_human = humans[k];
								double distance = human->position.distance(other_human.position);

								if (
									other_human.susceptible()
									&& distance <= settings.HUMAN_SPREAD_RANGE
									&& random_float() < settings.HUMAN_SPREAD_PROBABILITY
								) {
									new_infections.push_back(&other_human);
								}
							}
						}

						// Recover after HUMAN_INFECTION_DURATION ticks

						if (human->ticks_infected >= settings.HUMAN_INFECTION_DURATION) {
							human->infected = false;
							human->recovered = true;
						}
					}

					// Travel

					if (random_float() < settings.HUMAN_TRAVEL_RATIO) {
						int offset = travel_rng.generate();
						int new_community_id =
							(human->community_id + offset) % settings.NUMBER_OF_COMMUNITIES;

						move_human(human->community_id, j, new_community_id);
					}
				});

				// for (int j = 0; j < humans.size(); j++) {
				// 	Human *human = humans[j];

				// 	// Move the Human

				// 	Vector<2> acceleration = Vector<2>::from_angle(random_float() * 2 * M_PI);
				// 	human->move(acceleration);

				// 	// Infect other Humans if this Human is infected

				// 	if (human->infected) {
				// 		human->ticks_infected++;

				// 		// Infect each other Human that is within the HUMAN_SPREAD_RANGE,
				// 		// with a probability of HUMAN_SPREAD_PROBABILITY

				// 		// Todo: add incubation period here as well

				// 		for (int k = 0; k < humans.size(); k++) {
				// 			if (j != k) {
				// 				Human *other_human = humans[k];
				// 				double distance = human->position.distance(other_human->position);

				// 				if (
				// 					other_human->susceptible()
				// 					&& distance <= settings.HUMAN_SPREAD_RANGE
				// 					&& random_float() < settings.HUMAN_SPREAD_PROBABILITY
				// 				) {
				// 					new_infections.push_back(other_human);
				// 				}
				// 			}
				// 		}

				// 		// Recover after HUMAN_INFECTION_DURATION ticks

				// 		if (human->ticks_infected >= settings.HUMAN_INFECTION_DURATION) {
				// 			human->infected = false;
				// 			human->recovered = true;
				// 		}
				// 	}

				// 	// Travel

				// 	if (random_float() < settings.HUMAN_TRAVEL_RATIO) {
				// 		int offset = travel_rng.generate();
				// 		int new_community_id =
				// 			(human->community_id + offset) % settings.NUMBER_OF_COMMUNITIES;

				// 		move_human(human->community_id, j, new_community_id);
				// 	}
				// }
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
				vector<Human>& humans = communities[i];

				// Loop over each Human on the current community

				for (int j = 0; j < humans.size(); j++) {
					// Write the current Human to the FileBuffer

					buffer.write(humans[j].as_file_buffer());
				}
			}

			file.write(buffer.data(), buffer.size());
		}
};

#endif
