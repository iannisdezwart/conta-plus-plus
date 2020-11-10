#ifndef POPULATION_HEADER
#define POPULATION_HEADER

#include <bits/stdc++.h>
#include "human.hpp"
#include "filebuffer.hpp"
using namespace std;

extern const int COMMUNITY_SIZE;

class Population {
	public:
		FILE *file;

		int size;
		int communities;

		int tick_count = 0;

		Human *humans;

		Population(int size, int communities, string output_file_name)
		{
			file = fopen(output_file_name.c_str(), "w");

			this->size = size;
			this->communities = communities;

			// Allocate space for the humans

			humans = (Human *) malloc(sizeof(Human) * size);

			// Initialise each human

			for (int i = 0; i < size; i++) {
				// Calculate a random starting position

				Vector<2> starting_position({
					random_float() * COMMUNITY_SIZE,
					random_float() * COMMUNITY_SIZE
				});

				// Calculate a random community id

				uint16_t starting_community = random_float() * communities;

				humans[i] = Human(starting_position, starting_community);
			}

			write_file_header();
			write_tick_to_file();
		}

		void tick()
		{
			tick_count++;

			// Move each human

			for (int i = 0; i < size; i++) {
				humans[i].move();
				Vector<2> human_position = humans[i].position;
			}

			write_tick_to_file();
		}

		void write_file_header()
		{
			FileBuffer buffer;

			buffer.write("CONTA\n");
			buffer.write(size);
			buffer.write(communities);
			buffer.write('\n');

			fwrite(buffer.data(), 1, buffer.size(), file);
		}

		void write_tick_to_file()
		{
			FileBuffer buffer;

			for (int i = 0; i < size; i++) {
				buffer.write(humans[i].as_file_buffer());
			}

			fwrite(buffer.data(), 1, buffer.size(), file);
		}
};

#endif