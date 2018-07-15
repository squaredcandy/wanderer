#pragma once

#include "Reflection.h"
#include "Texture.h"
#include "Camera.h"
#include "Entities.h"
#include "Dungeon.h"
#include "Flags.h"

#include <chrono>
#include <array>
#include <vector>
#include <string>
#include <numeric>

#include <ImGui/imgui.h>
#include <GL/gl3w.h>

const int LOD_LENGTH = 5;
const int DEBUG_LENGTH = 100;
const float NANO_TO_MILLI = 1000000.0f;

const int TIMER_GRAPHICS = 0;
const int TIMER_UI = 1;
const int TIMER_UPDATE = 2;
const int TIMER_TOTAL = 3;
const int TIMER_LENGTH = 4;

// High Resolution Clock
using HR_Clock = std::chrono::high_resolution_clock;
using NS_Time = std::chrono::nanoseconds;

using DebugArray = std::array<float, DEBUG_LENGTH>;

struct DebugData
{
	int x, y;
	float heightFactor;

	float bias;
	int samples;
	
	std::array<float, LOD_LENGTH> lodDist;
	std::array<float, LOD_LENGTH> tesLevel;

	struct DirectionalLight
	{
		glm::vec3 ambient;
		float aIntensity;

		glm::vec3 direction;
		float dIntensity;
	};

	DirectionalLight dLight;

	DebugData()
	{
		x = 0, y = 0;
		bias = 2.f;
		samples = 20;
		//drawArrays = true;
		heightFactor = 0;
		
		dLight.ambient = { 1.0f, 1.f, 1.f };
		dLight.aIntensity = 0.5f;

		dLight.direction = { 0, 1, 0 };
		dLight.dIntensity = 0.25f;

		lodDist = { 50, 100, 500, 1000, 10000 };
		tesLevel = { 30, 20, 12, 4, 2 };
	}

	~DebugData() = default;
};

namespace Wanderer::Engine::World
{
	extern bool drawGridLines;
	extern float gridLineWidth;
	extern float terrainScale;
	extern int heightSeed;
	extern int chunkSize;

	void RandomiseSeed();
	void GenerateWorld(int numOfChunks);
}

namespace Wanderer::Engine::Debug
{
	extern DebugData data;

	extern int renderTimeIdx;
	extern std::array<DebugArray, TIMER_LENGTH> debugTimers;

	void UpdateTime(int timer, HR_Clock::time_point startTime);
	void DrawGraph(std::string name, DebugArray& timer, ImVec2 size = { 0, 80 });
	void DebugWindow();
}