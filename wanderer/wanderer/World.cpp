#include "World.h"

namespace Wanderer::Engine::World
{
	float terrainScale	= 100;

	int chunkSize		= 256;

	int heightSeed		= 1337;

	int octaves			= 3;

	float gain			= 0.5f;
	float increase		= 0.1f;
	float lacunarity	= 2.0f;
	float frequency		= 0.01f;

	FastNoise::Interp interp		= FastNoise::Quintic;
	FastNoise::NoiseType noiseType	= FastNoise::PerlinFractal;
	FastNoise::FractalType fracType	= FastNoise::FBM;

	bool drawGridLines	= true;
	float gridLineWidth	= 0.99f;

	std::vector<Chunk> chunks;

	void RandomiseSeed()
	{
		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_int<int> dist(0, 1337);
		heightSeed = abs(dist(rng));
	}

	void GenerateMap(Chunk& chunk, FastNoise& fn)
	{
		chunk.chunk.resize(chunkSize * chunkSize);
		// No offsets for now, just draw the first one
		//offset /= TILESIZE * 10;
		for (int i = 0; i < chunkSize; ++i)
		{
			for (int j = 0; j < chunkSize; ++j)
			{
				//auto x = (FN_DECIMAL) (i + (int) offset[0]);
				//auto y = (FN_DECIMAL) (j + (int) offset[2]);
				auto x = (FN_DECIMAL) i;
				auto y = (FN_DECIMAL) j;
				chunk.chunk[i + j * chunkSize] = fn.GetNoise(x, y) + increase;
				//loadingNum++;
			}
		}
	}

	void GenerateWorld(int numOfChunks)
	{
		chunks.clear();
		for (int i = 0; i < numOfChunks; ++i)
		{
			Chunk c;
			chunks.emplace_back(c);
		}

		FastNoise fnHeight;
		fnHeight.SetNoiseType(noiseType);
		fnHeight.SetFrequency(frequency);
		fnHeight.SetFractalType(fracType);
		fnHeight.SetFractalOctaves(octaves);
		fnHeight.SetFractalLacunarity(lacunarity);
		fnHeight.SetFractalGain(gain);
		fnHeight.SetInterp(interp);
		fnHeight.SetSeed(heightSeed);

		for (auto& chunk : chunks)
		{
			GenerateMap(chunk, fnHeight);
			Textures::LoadWorldMap(CHUNK_TERRAIN, chunk.chunk, chunkSize);
		}
	}
}