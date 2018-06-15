#include "Debug.h"

namespace Wanderer::Engine::Debug
{
	DebugData debugData;

	int renderTimeIdx = 0;
	std::array<DebugArray, TIMER_LENGTH> debugTimers;

	void UpdateTime(DebugArray& timer, HR_Clock::time_point startTime)
	{
		timer[renderTimeIdx] = (float) std::chrono::duration_cast<NS_Time>
			(HR_Clock::now() - startTime).count() / NANO_TO_MILLI;
	}

	void DrawGraph(std::string name, DebugArray& timer, ImVec2 size)
	{
		auto avg = std::accumulate(timer.begin(), timer.end(), 0.f) / (float)timer.size();
		auto max = *std::max_element(timer.begin(), timer.end());
		auto min = *std::min_element(timer.begin(), timer.end());
		std::string des = (name + 
			"\nAvg: " + std::to_string(avg) + " ms"
			"\nMax: " + std::to_string(max) + " ms"
			"\nMin: " + std::to_string(min) + " ms"
		);
		ImGui::PlotLines(
			des.c_str(), timer.data(), DEBUG_LENGTH, renderTimeIdx,
			(std::to_string(timer[renderTimeIdx]) +
			 std::string(" ms")).c_str(), 0.f, max + 1.f, size
		);
	}

	void DebugWindow()
	{
		ImGui::Checkbox("Wireframe", &debugData.wireframe);
		if (debugData.wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		ImGui::SliderFloat("tVal", &debugData.tVal, 0.f, 1.f);
	}

}