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

#ifdef MULTI_THREADED
using BoostThreadPool = boost::asio::thread_pool;
#endif

const int NUM_OF_THREADS = thread::hardware_concurrency();

class Population {
	public:
		fs::File& file;
		SimulationSettings& settings;

		vector<vector<Human *>> communities;
		int tick_count = 0;
		bool social_distancing = false;

		#ifdef MULTI_THREADED
		atomic<int> infected_count;
		#else
		int infected_count;
		#endif

		RandomIntGenerator travel_rng;
		RandomIntGenerator community_rng;

		#ifdef MULTI_THREADED
		BoostThreadPool thread_pool;
		#endif

		Population(fs::File& output_file, SimulationSettings& simulation_settings)
			: settings(simulation_settings),
				file(output_file),
				travel_rng(1, settings.NUMBER_OF_COMMUNITIES - 1),
				community_rng(0, settings.NUMBER_OF_COMMUNITIES - 1)
				#ifdef MULTI_THREADED
				,thread_pool(NUM_OF_THREADS)
				#endif
		{
			// Initialise (atomic) int

			infected_count = 0;

			// Create all community vectors

			for (int i = 0; i < settings.NUMBER_OF_COMMUNITIES; i++) {
				vector<Human *> humans_on_this_community;
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

				Human *human = new Human(starting_position, starting_community, settings);

				// Infect this Human if it its index matches

				if (i == infected_human_index) {
					human->infected = true;
					infected_count++;
				}

				// Add the human to the correct community

				communities[starting_community].push_back(human);
			}

			// Write the file header...

			write_file_header();

			// ...and the starting positions of the humans

			write_tick_to_file();
		}

		#ifdef MULTI_THREADED

		void loop_over_community(int com_i, function<void (Human *, int)> callback)
		{
			Population *pop = this;
			atomic<int> finished_threads;
			finished_threads = 0;

			for (int thr_i = 0; thr_i < NUM_OF_THREADS; thr_i++) {
				boost::asio::post(thread_pool,
					[com_i, thr_i, pop, callback, &finished_threads]() {
						int i = thr_i;
						int com_size = pop->communities[com_i].size();

						while (i < com_size) {
							callback(pop->communities[com_i][i], i);
							i += NUM_OF_THREADS;
						}

						finished_threads++;
				});
			}

			while (finished_threads != NUM_OF_THREADS) {
				this_thread::sleep_for(chrono::nanoseconds(1000));
			}
		}

		void loop_over_population(function<void (Human *, int, int)> callback)
		{
			Population *pop = this;
			atomic<int> fin;
			fin = 0;

			for (int com_i = 0; com_i < settings.NUMBER_OF_COMMUNITIES; com_i++) {
				boost::asio::post(thread_pool, [com_i, pop, &callback, &fin]() {
					for (int i = 0; i < pop->communities[com_i].size(); i++) {
						callback(pop->communities[com_i][i], com_i, i);
					}

					fin++;
				});
			}

			while (fin != settings.NUMBER_OF_COMMUNITIES) {
				this_thread::sleep_for(chrono::nanoseconds(1000));
			}
		}

		#else

		void loop_over_community(int com_i, function<void (Human *, int)> callback)
		{
			for (int i = 0; i < communities[com_i].size(); i++) {
				callback(communities[com_i][i], i);
			}
		}

		void loop_over_population(function<void (Human *, int, int)> callback)
		{
			for (int com_i = 0; com_i < settings.NUMBER_OF_COMMUNITIES; com_i++) {
				for (int i = 0; i < communities[com_i].size(); i++) {
					callback(communities[com_i][i], com_i, i);
				}
			}
		}

		#endif

		~Population()
		{
			#ifdef MULTI_THREADED

			// Join the threads

			thread_pool.join();

			#endif

			// Delete the allocated Humans

			printf("Deleting population...\n");

			for (int com_i = 0; com_i < settings.NUMBER_OF_COMMUNITIES; com_i++) {
				for (int i = 0; i < communities[com_i].size(); i++) {
					delete communities[com_i][i];
				}
			}
		}

		void move_human(uint8_t old_community_id, int index, uint8_t new_community_id)
		{
			Human *human = communities[old_community_id][index];

			// Set human community id

			human->community_id = new_community_id;

			// Delete human from the old community

			vector<Human *>& community = communities[old_community_id];
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

			// Enable social distancing if the infected count reaches the threshold

			if (infected_count >= settings.SOCIAL_DISTANCING_THRESHOLD)
				social_distancing = true;

			// Disable social distancing if the infected count reaches the release

			if (social_distancing
				&& infected_count < settings.SOCIAL_DISTANCING_RELEASE)
					social_distancing = false;

			// Loop over each human in the population

			loop_over_population([&](Human *human, int com_i, int i) {
				// Move the Human

				Vector<2> acc = Vector<2>::from_angle(random_float() * 2 * M_PI);

				if (social_distancing && random_float() < settings.SOCIAL_DISTANCING_RATIO) {
					// Keep distance from other Humans

					for (int j = 0; j < communities[com_i].size(); j++) {
						if (i != j) {
							Vector<2>& pos = human->position;
							Vector<2>& other_pos = communities[com_i][j]->position;

							if (pos.distance(other_pos) < settings.SOCIAL_DISTANCING_RADIUS) {
								acc += pos - other_pos;
							}
						}
					}
				}

				human->move(acc);

				// Handle infection

				if (human->infected) {
					vector<Human *>& humans = communities[com_i];
					human->ticks_infected++;

					// After the incubation period ...

					if (human->ticks_infected > settings.HUMAN_INCUBATION_PERIOD) {
						human->incubating = false;

						// ... infect each other Human that is within the HUMAN_SPREAD_RANGE,
						// with a probability of HUMAN_SPREAD_PROBABILITY

						for (int k = 0; k < humans.size(); k++) {
							if (i != k) {
								Human *other_human = humans[k];

								if (!other_human->susceptible()) continue;

								double distance = human->position.distance(other_human->position);

								if (
									distance <= settings.HUMAN_SPREAD_RANGE
									&& random_float() < settings.HUMAN_SPREAD_PROBABILITY
								) {
									other_human->infected = true;
									infected_count++;
									other_human->incubating = true;
								}
							}
						}
					}


					// Recover after HUMAN_INFECTION_DURATION ticks

					if (human->ticks_infected >= settings.HUMAN_INFECTION_DURATION) {
						human->infected = false;
						infected_count--;
						human->recovered = true;
					}
				}
			});

			// Consider traveling for each Human

			for (int com_i = 0; com_i < settings.NUMBER_OF_COMMUNITIES; com_i++) {
				for (int i = 0; i < communities[com_i].size(); i++) {
					if (random_float() < settings.HUMAN_TRAVEL_RATIO) {
						int offset = travel_rng.generate();
						int new_com_id = (com_i + offset) % settings.NUMBER_OF_COMMUNITIES;
						move_human(com_i, i, new_com_id);
					}
				}
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
				vector<Human *>& humans = communities[i];

				// Loop over each Human on the current community

				for (int j = 0; j < humans.size(); j++) {
					// Write the current Human to the FileBuffer

					buffer.write(humans[j]->as_file_buffer());
				}
			}

			file.write(buffer.data(), buffer.size());
		}
};

#endif
