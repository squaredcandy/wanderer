#pragma once

#include "Texture.h"
#include "Random.h"

#include <random>
#include <vector>
//#include <map>
//#include <atomic>

#include <glm/glm.hpp>
#include <FastNoise/FastNoise.h>

const TextureID CHUNK_TERRAIN = 0;

struct Chunk
{
	// Chunks offset from the centre
	glm::vec3 offset;
	// Chunk Values
	std::vector<float> chunk;
};

namespace Wanderer::Engine::World
{
	extern bool drawGridLines;
	extern float gridLineWidth;

	extern float terrainScale;

	extern int chunkSize;

	extern int heightSeed;

	extern int octaves;

	extern float gain;
	extern float increase;
	extern float lacunarity;
	extern float frequency;

	extern FastNoise::Interp interp;
	extern FastNoise::NoiseType noiseType;
	extern FastNoise::FractalType fracType;

	extern bool drawGridLines;
	extern float gridLineWidth;

	extern std::vector<Chunk> chunks;

	void RandomiseSeed();
	void GenerateWorld(int numOfChunks);
}