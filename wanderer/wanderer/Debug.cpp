#include "Debug.h"

namespace Wanderer::Engine::Debug
{
	DebugData debugData;

	int renderTimeIdx = 0;
	std::array<DebugArray, TIMER_LENGTH> debugTimers;

	void UpdateTime(int timer, HR_Clock::time_point startTime)
	{
		debugTimers[timer][renderTimeIdx] = 
			(float) std::chrono::duration_cast<NS_Time>
			(HR_Clock::now() - startTime).count() / NANO_TO_MILLI;
	}

	void DrawGraph(std::string name, DebugArray& timer, ImVec2 size)
	{
		auto avg = std::accumulate(timer.begin(), timer.end(), 0.f) 
			/ (float)timer.size();
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
			 std::string(" ms")).c_str(), 0.f, max + 1.f, size);
	}

	void DebugWindow()
	{
		if (debugData.wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Show Framerate / Frametime
		auto& timer = debugTimers[TIMER_TOTAL];
		float frameTime = std::accumulate(timer.begin(), timer.end(), 0.f) 
			/ (float) timer.size();
		float frameRate = 1 / (frameTime / 1000);

		ImGui::Text("Frame Time: %.3f ms", frameTime);
		ImGui::Text("Frame Rate: %.3f fps", frameRate);
		if (ImGui::CollapsingHeader("Camera"))
		{
			auto& camData = Camera::GetCamera();
			EDITVALUEN(camData.translateSpeed, "Translation Speed");
		}
		if (ImGui::CollapsingHeader("Light"))
		{
			ImGui::ColorEdit3("Ambient Light", &debugData.dLight.ambient.x);
			ImGui::SliderFloat("Ambient Intensity", &debugData.dLight.aIntensity, 0.f, 1.f);

			ImGui::DragFloat3("Diffuse Direction", &debugData.dLight.direction.x);
			ImGui::SliderFloat("Diffuse Intensity", &debugData.dLight.dIntensity, 0.f, 1.f);
		}
		if (ImGui::CollapsingHeader("Terrain"))
		{
			EDITVALUEN(World::terrainScale, "Terrain Scale");

			ImGui::Checkbox("Draw Grid Lines", &World::drawGridLines);
			ImGui::SliderFloat("Grid Line Width", &World::gridLineWidth, 0.f, 1.f);

			ImGui::PushItemWidth(200);
			ImGui::InputInt("Height Seed", &World::heightSeed);
			ImGui::PopItemWidth();
			ImGui::SameLine();
			if (ImGui::Button("Randomise"))
			{
				World::RandomiseSeed();
			}
			ImGui::PushItemWidth(200);
			ImGui::InputInt("Chunk Size", &World::chunkSize, World::chunkSize, World::chunkSize);
			ImGui::PopItemWidth();

			if (ImGui::Button("Regenerate World"))
			{
				World::GenerateWorld(1);
			}
			if (ImGui::TreeNode("Height Map"))
			{
				// Show Heightmap
				auto heightmap = Textures::GetMaterial(0);
				ImGui::Image((void*)heightmap->textures[Material::MAP_HEIGHT]->textureID, 
							 { 128, 128 });
				ImGui::TreePop();
			}
			ImGui::Separator();

			EDITVALUEN(debugData.wireframe, "Wireframe");
			EDITVALUEN(debugData.drawArrays, "Draw Arrays");
			EDITVALUEN(debugData.heightFactor, "Height Factor");
		}
	}
}