#include "Debug.h"

namespace Wanderer::Engine::Debug
{
	int renderTimeIdx = 0;
	std::array<DebugArray, TIMER_LENGTH> debugTimers;

	void UpdateTime(DebugArray& timer, HR_Clock::time_point startTime)
	{
		timer[renderTimeIdx] = (float) std::chrono::duration_cast<NS_Time>
			(HR_Clock::now() - startTime).count() / NANO_TO_MILLI;
	}

	void DrawGraph(std::string name, DebugArray& timer, ImVec2 size)
	{
		ImGui::PlotLines(
			name.c_str(), timer.data(),
			DEBUG_LENGTH, renderTimeIdx,
			(std::to_string(timer[renderTimeIdx]) +
			 std::string(" ms")).c_str(), 0.f,
			*std::max_element(timer.begin(), timer.end()) + 1.f,
			size
		);
	}

}