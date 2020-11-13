#ifndef HUMAN_HEADER
#define HUMAN_HEADER

#include <bits/stdc++.h>

#include "libs/linear_algebra.hpp"
#include "random.hpp"
#include "filebuffer.hpp"
#include "simulation_settings.hpp"

class Population;

using namespace std;

class Human {
	public:
		SimulationSettings& settings;

		Vector<2> position;
		Vector<2> velocity;
		Vector<2> acceleration;

		bool infected = false;
		int ticks_infected = 0;
		bool recovered = false;

		uint16_t community_id;

		Human(Vector<2> starting_position, uint16_t starting_community, SimulationSettings& simulation_settings) : settings(simulation_settings)
		{
			position = starting_position;
			velocity.nullify();
			acceleration.nullify();

			community_id = starting_community;
		}

		void move(Vector<2> new_acceleration)
		{
			// Generate random acceleration

			acceleration = new_acceleration;

			// Calculate velocity

			velocity += acceleration;

			// Cap maximum velocity

			if (velocity.length() > settings.HUMAN_MAX_VELOCITY) {
				velocity.normalise();
				velocity *= settings.HUMAN_MAX_VELOCITY;
			}

			// Calculate new position

			position += velocity;

			// Bound check

			if (position[0] < 0) {
				position[0] = COMMUNITY_SIZE - 1;
			}

			if (position[0] > COMMUNITY_SIZE - 1) {
				position[0] = 0;
			}

			if (position[1] < 0) {
				position[1] = COMMUNITY_SIZE - 1;
			}

			if (position[1] > COMMUNITY_SIZE - 1) {
				position[1] = 0;
			}
		}

		bool susceptible()
		{
			return !(infected | recovered);
		}

		// Returns a FileBuffer that describes the Human in the CONTA file format

		FileBuffer as_file_buffer()
		{
			FileBuffer buffer;

			// Format:
			// [ uint16 COMMUNITY_ID ]
			// [ uint16 POSITION_X ]
			// [ uint16 POSITION_Y ]
			// [ uint8 FLAGS ] ( 0 0 0 0 0 0 RECOVERED INFECTED )

			buffer.write((uint16_t) community_id);
			buffer.write((uint16_t) position[0]);
			buffer.write((uint16_t) position[1]);

			uint8_t flags = (recovered << 1) | (infected << 0);
			buffer.write(flags);

			return buffer;
		}
};

#endif
