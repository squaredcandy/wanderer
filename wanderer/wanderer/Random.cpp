#include "Random.h"

namespace Wanderer::Engine::Random
{
	std::random_device rd;
	std::mt19937 mt(rd());

	int RandomInt(int exclusiveMax)
	{
		std::uniform_int_distribution<> dist(0, exclusiveMax - 1);
		return dist(mt);
	}

	int RandomInt(int min, int max) // inclusive min/max
	{
		std::uniform_int_distribution<> dist(0, max - min);
		return dist(mt) + min;
	}

	bool RandomBool(double probability)
	{
		std::bernoulli_distribution dist(probability);
		return dist(mt);
	}
}