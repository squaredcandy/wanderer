#pragma once

#include <random>

namespace Wanderer::Engine::Random
{
	int RandomInt(int exclusiveMax);
	int RandomInt(int min, int max); // inclusive min/max
	bool RandomBool(double probability = 0.5);
}