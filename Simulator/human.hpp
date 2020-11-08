#ifndef HUMAN_HEADER
#define HUMAN_HEADER

#include <bits/stdc++.h>

#include "libs/linear_algebra.hpp"
#include "random.hpp"
#include "filebuffer.hpp"

using namespace std;

extern const int COMMUNITY_SIZE;

class Human {
	public:
		Vector<2> position;
		Vector<2> velocity;
		Vector<2> acceleration;

		static const int max_velocity = 5;

		Human(Vector<2> starting_position)
		{
			position = starting_position;
			velocity.nullify();
			acceleration.nullify();
		}

		void move()
		{
			// Generate random acceleration

			acceleration = Vector<2>::from_angle(random_float() * 2 * M_PI);

			// Calculate velocity

			velocity += acceleration;

			// Cap maximum velocity

			if (velocity.length() > max_velocity) {
				velocity.normalise();
				velocity *= max_velocity;
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

		// Returns a FileBuffer that describes the Human in the CONTA file format

		FileBuffer as_file_buffer()
		{
			FileBuffer buffer;

			// Format:
			// [ uint16 POSITION_X ]
			// [ uint16 POSITION_Y ]

			buffer.write((uint16_t) position[0]);
			buffer.write((uint16_t) position[1]);

			return buffer;
		}
};

#endif