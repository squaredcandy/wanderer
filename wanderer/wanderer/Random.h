#pragma once

#include <random>

namespace Wanderer::Engine::Random
{
	int RandomInt(int exclusiveMax = 2);
	int RandomInt(int min, int max); // inclusive min/max
	float RandomFloat(float exclusiveMax = 2.f);
	float RandomFloat(float min, float max); // inclusive min/max
	bool RandomBool(double probability = 0.5);
}