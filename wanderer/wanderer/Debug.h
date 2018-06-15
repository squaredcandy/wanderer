#pragma once

#include <chrono>
#include <array>
#include <vector>
#include <string>
#include <numeric>

#include <ImGui/imgui.h>
#include <GL/gl3w.h>

const int DEBUG_LENGTH = 100;
const float NANO_TO_MILLI = 1000000.0f;

const int TIMER_GRAPHICS = 0;
const int TIMER_UI = 1;
const int TIMER_UPDATE = 2;
const int TIMER_LENGTH = 3;

// High Resolution Clock
using HR_Clock = std::chrono::high_resolution_clock;
using NS_Time = std::chrono::nanoseconds;

using DebugArray = std::array<float, DEBUG_LENGTH>;

struct DebugData
{
	bool wireframe;
	float tVal;
};

namespace Wanderer::Engine::Debug
{
	extern DebugData debugData;

	extern int renderTimeIdx;
	extern std::array<DebugArray, TIMER_LENGTH> debugTimers;

	void UpdateTime(DebugArray& timer, HR_Clock::time_point startTime);
	void DrawGraph(std::string name, DebugArray& timer, ImVec2 size = { 0, 80 });
	void DebugWindow();
}