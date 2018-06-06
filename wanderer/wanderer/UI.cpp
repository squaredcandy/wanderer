#include "UI.h"

namespace Wanderer::Game::UI
{
	void DebugGraphWindow()
	{
		ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiSetCond_FirstUseEver);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Debug Graph Window", &Engine::Flag::showDebugGraphWindow);

		Engine::Debug::DrawGraph("Render Graph", Engine::Debug::debugTimers[TIMER_GRAPHICS]);
		Engine::Debug::DrawGraph("UI Graph", Engine::Debug::debugTimers[TIMER_UI]);
		Engine::Debug::DrawGraph("Update Graph", Engine::Debug::debugTimers[TIMER_UPDATE]);

		ImGui::End();
		ImGui::PopStyleVar();
	}
}