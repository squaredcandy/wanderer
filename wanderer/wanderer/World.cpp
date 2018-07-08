#include "World.h"

#include <lodepng/lodepng.h>

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

	bool drawGridLines	= false;
	float gridLineWidth = 0.99f;

	std::vector<Chunk> chunks;

	void RandomiseSeed()
	{
		heightSeed = Random::RandomInt(1337);
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
				auto pos = i + j * chunkSize;

// 				const float offset = 1.f;
// 				float hL = fn.GetNoise(x - offset, y);
// 				float hR = fn.GetNoise(x + offset, y);
// 				float hD = fn.GetNoise(x, y - offset);
// 				float hU = fn.GetNoise(x, y + offset);
// 
// 				glm::vec3 n;
// 				n.x = hL - hR;
// 				n.y = hD - hU;
// 				n.z = 1.0;
// 				n = glm::normalize(n);
// 
// 				chunk.chunk[pos].r = n.x;
// 				chunk.chunk[pos].g = n.z;
// 				chunk.chunk[pos].b = n.y;
				chunk.chunk[pos] = fn.GetNoise(x, y) + increase;
				//chunk.chunk[pos].a = 1.0f;
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

			std::vector<unsigned char> a;

			a.reserve(chunk.chunk.size() * 4);

			for (auto& val : chunk.chunk)
			{
				unsigned char b = val >= 0 ? val * 255 : 0;
				a.emplace_back(b);
				a.emplace_back(b);
				a.emplace_back(b);
				a.emplace_back(255);
			}
			unsigned error = lodepng::encode("test.png", a, chunkSize, chunkSize);
			if (error)
			{
				printf("Encode Error: %u %s\n", error, lodepng_error_text(error));
			}
		}
	}
}