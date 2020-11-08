#ifndef POPULATION_HEADER
#define POPULATION_HEADER

#include <bits/stdc++.h>
#include "human.hpp"
#include "filebuffer.hpp"
#include "libs/png++-0.2.9/png.hpp"

using namespace std;

extern const int COMMUNITY_SIZE;

class Population {
	public:
		FILE *file;

		int size;
		int communities;

		int tick_count = 0;

		Human *humans;

		png::image<png::rgb_pixel> *image;

		Population(int size, int communities, string output_file_name)
		{
			file = fopen(output_file_name.c_str(), "w");

			this->size = size;
			this->communities = communities;

			image = new png::image<png::rgb_pixel>(COMMUNITY_SIZE, COMMUNITY_SIZE);

			// Allocate space for the humans

			humans = (Human *) malloc(sizeof(Human) * size);

			// Initialise each human

			for (int i = 0; i < size; i++) {
				Vector<2> starting_position({
					random_float() * COMMUNITY_SIZE,
					random_float() * COMMUNITY_SIZE
				});

				humans[i] = Human(starting_position);
			}

			// render();
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

			// render();
			write_tick_to_file();
		}

		void render()
		{
			// Clear image

			for (int y = 0; y < COMMUNITY_SIZE; y++) {
				for (int x = 0; x < COMMUNITY_SIZE; x++) {
					image->set_pixel(x, y, { 255, 255, 255 });
				}
			}

			// Draw image

			for (int i = 0; i < size; i++) {
				int x = (int) humans[i].position[0];
				int y = (int) humans[i].position[1];

				image->set_pixel(x, y, { 255, 0, 0 });
				if (x + 1 < COMMUNITY_SIZE) image->set_pixel(x + 1, y, { 255, 0, 0 });
				if (x - 1 > 0) image->set_pixel(x - 1, y, { 255, 0, 0 });
				if (y + 1 < COMMUNITY_SIZE) image->set_pixel(x, y + 1, { 255, 0, 0 });
				if (y - 1 > 0) image->set_pixel(x, y - 1, { 255, 0, 0 });
			}

			string frame_number = to_string(tick_count);
			frame_number = string(4 - frame_number.length(), '0') + frame_number;
			image->write("output/" + frame_number + ".png");
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

			buffer.write('\n');

			fwrite(buffer.data(), 1, buffer.size(), file);
		}
};

#endif