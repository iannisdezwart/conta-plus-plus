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
		bool incubating = false;
		bool recovered = false;
		int ticks_recovered = 0;

		uint16_t community_id;

		Human(Vector<2> starting_position, uint16_t starting_community, SimulationSettings& simulation_settings) : settings(simulation_settings)
		{
			position = starting_position;
			velocity.nullify();
			acceleration.nullify();

			community_id = starting_community;
		}

		void operator=(const Human& human)
		{
			position = human.position;
			velocity = human.velocity;
			acceleration = human.acceleration;

			infected = human.infected;
			ticks_infected = human.ticks_infected;
			recovered = human.recovered;

			community_id = human.community_id;
		}

		void move(Vector<2>& new_acceleration)
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
			// [ uint32 HUMAN ]:
				// [ uint11 COMMUNITY_ID ]
				// [ uint9 POSITION_X ]
				// [ uint9 POSITION_Y ]
				// [ uint3 FLAGS ] ( INCUBATING RECOVERED INFECTED )

			uint32_t human = 0;

			// Write the community ID to bytes [ 31 - 21 ]

			human |= community_id << 21;

			// Write the X and Y positions to bytes [ 20 - 12 ] and [ 11 - 3 ]

			human |= ((uint16_t) position[0] << 12);
			human |= ((uint16_t) position[1] << 3);

			// Write the flags to bytes [ 2 - 0 ]

			uint8_t flags = (incubating << 2) | (recovered << 1) | (infected << 0);
			human |= flags;

			buffer.write(human);

			return buffer;
		}
};

#endif
