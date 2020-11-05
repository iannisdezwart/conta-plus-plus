#ifndef RANDOM_HEADER
#define RANDOM_HEADER

#include <bits/stdc++.h>

using namespace std;

template <uint64_t lower_bound, uint64_t upper_bound>
class RandomIntGenerator {
	private:
		random_device device;
		mt19937 generator;
		uniform_int_distribution<uint64_t> distribution;

	public:
		RandomIntGenerator()
		{
			generator = mt19937(device());
			distribution = uniform_int_distribution<uint64_t>(lower_bound, upper_bound);
		}

		uint64_t generate()
		{
			return distribution(generator);
		}

		RandomIntGenerator& operator=(RandomIntGenerator const& other)
		{
			return *this = other;
		}
};

class RandomFloatGenerator {
	private:
		RandomIntGenerator<0, 9223372036854775807> *random_int_generator;

	public:
		RandomFloatGenerator()
		{
			random_int_generator = new RandomIntGenerator<0, 9223372036854775807>();
		}

		double generate()
		{
			return (double) random_int_generator->generate() / 9223372036854775807.0;
		}
};

RandomFloatGenerator random_float_generator;

double random_float()
{
	return random_float_generator.generate();
}

#endif