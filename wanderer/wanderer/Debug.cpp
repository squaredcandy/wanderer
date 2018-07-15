#include "Debug.h"

namespace Wanderer::Engine::Debug
{
	DebugData data;

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
		if (Flag::wireframeMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Show Framerate / Frametime
		auto& timer = debugTimers[TIMER_TOTAL];
		float frameTime = std::accumulate(timer.begin(), timer.end(), 0.f) 
			/ (float) timer.size();
		float frameRate = 1 / (frameTime / 1000);

		ImGui::Text("Frame Time: %.3f ms", frameTime);
		ImGui::Text("Frame Rate: %.3f fps", frameRate);
		ImGui::Checkbox("Debug Mode", &Flag::debugMode);
		ImGui::Checkbox("Wireframe Mode", &Flag::wireframeMode);
		ImGui::DragFloat("height Scale", &data.heightFactor, 0.01f);
		if (ImGui::CollapsingHeader("Camera"))
		{
			auto& camData = Camera::GetCamera();
			EDITVALUEN(camData.translateSpeed, "Translation Speed");
		}
		if (ImGui::CollapsingHeader("Dungeon"))
		{
			for (auto& map : Dungeon::dMap)
			{
				auto& m = map.second;
				for (int y = 0; y < m.height; ++y)
				{
					std::string line;
					for (int x = 0; x < m.width; ++x)
					{
						line += (char) Dungeon::GetTile(x, y);
					}
					ImGui::Text(line.c_str());
				}
			}
		}
		if (ImGui::CollapsingHeader("Light"))
		{
			static std::string currentLightName{ "" };
			static EntityID currentID = -1;
			static Entity * currentLight;
			ImGui::DragFloat("Bias", &data.bias, 0.01f);
			EDITVALUEN(data.samples, "Samples");
			const int nameLen = 20;
			static std::array<char, nameLen> newLightName;
			ImGui::PushItemWidth(200);
			ImGui::InputText("New Light Name", newLightName.data(), nameLen);
			ImGui::SameLine();
			if (ImGui::Button("Add Light"))
			{
				if (!newLightName.empty())
				{
					auto newEnt = Entities::AddEntity(newLightName.data());
					newEnt->AddMod<ModLight>();
					currentLight = newEnt;
					currentID = newEnt->GetID();
					currentLightName = newEnt->GetCommon().name;
				}
			}
			auto lights = Entities::GetAll<ModLight>();
			std::string name{ "light (" + std::to_string(lights.size()) + ")" };
			if (ImGui::BeginCombo(name.c_str(), currentLightName.c_str()))
			{
				for (auto& light : lights)
				{
					std::string lightName = light->GetCommon().name;
					EntityID id = light->GetID();
					bool selected = (currentID == id);
					ImGui::PushID((int)id);
					if (ImGui::Selectable(lightName.c_str(), selected))
					{
						currentLightName = lightName;
						currentID = id;
						currentLight = light;
					}
					if (selected)
					{
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}
				ImGui::EndCombo();
			}
			if (currentLight)
			{
				ImGui::SameLine();
				if (ImGui::Button("Delete Light"))
				{
					currentLight->GetMod<ModLight>().drawLight = false;
					currentLight->destroy();
					currentLightName = "";
					currentID = -1;
					currentLight = nullptr;
				}
				else
				{
					auto& l = currentLight->GetMod<ModLight>();
					auto& postion = currentLight->GetTransform().GetTranslate();
					EDITVALUEN(l.drawLight, "Draw Light");
					EDITVALUEN(postion, "Light Position");
					EDITVALUEN(l.color, "Light Color");
				}
			}
		}
		if (ImGui::CollapsingHeader("Entity"))
		{
			auto ent = Entities::GetAll();
			static std::string currentEntityName{ "" };
			static EntityID currentID = -1;
			static Entity* currentEntity;
			if (ImGui::BeginCombo("##Entity", currentEntityName.c_str()))
			{
				for (auto& e : ent)
				{
					std::string entityName = e->GetCommon().name;
					EntityID id = e->GetID();
					bool selected = (currentID = id);
					ImGui::PushID((int)id);
					if (ImGui::Selectable(entityName.c_str(), selected))
					{
						currentEntityName = entityName;
						currentID = id;
						currentEntity = e;
					}
					if (selected)
					{
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}
				ImGui::EndCombo();
			}
			if (currentEntity)
			{
				currentEntity->GUI();
			}
		}
		if (ImGui::CollapsingHeader("Material"))
		{
			static std::string currentID{ "" };
			static Material * currentMat;

			static Material::TextureType currentTT = Material::MAP_DIFFUSE;
			static Texture * currentTex;
			auto& mats = Textures::GetAllMaterials();
			ImGui::PushItemWidth(200);
			ImGui::PushID(100);
			if (ImGui::BeginCombo("", currentID.c_str()))
			{
				int i = 0;
				for (auto& mat : mats)
				{
					std::string id = mat.first;
					bool selected = (currentID == id);
					ImGui::PushID(i++);
					if (ImGui::Selectable(id.c_str(), selected))
					{
						currentID = id;
						currentMat = &mat.second;
						currentTex = nullptr;
					}
					if (selected)
					{
						ImGui::SetItemDefaultFocus();
					}
					ImGui::PopID();
				}
				ImGui::EndCombo();
			}
			ImGui::PopID();
			if (currentMat)
			{
				auto& texs = currentMat->textures;
				ImGui::SameLine();
				ImGui::PushItemWidth(200);
				ImGui::PushID(101);
				if (ImGui::BeginCombo("", texToString[currentTT].c_str()))
				{
					for (auto& tex : texs)
					{
						Material::TextureType tt = tex.first;
						if(tex.first == Material::MAP_CUBEMAP) continue;
						bool selected = (currentTT == tt);
						ImGui::PushID(tt);
						if (ImGui::Selectable(texToString[tt].c_str(), selected))
						{
							currentTT = tt;
							currentTex = &tex.second;
						}
						if (selected)
						{
							ImGui::SetItemDefaultFocus();
						}
						ImGui::PopID();
					}
					ImGui::EndCombo();
				}
				ImGui::PopID();
			}

			if (currentTex)
			{
				ImGui::Image((GLvoid*) (GLuint64) currentTex->textureID, { 128, 128 });
			}

		}

		static int height = 4;
		static int width = 4;
		static float totalTime = 1;
		static float animTimer = 0;

		ImGui::SliderFloat("Anim Time", &totalTime, 0.01f, 3);
		ImGui::SliderInt("x", &data.x, 0, width - 1);
		ImGui::SliderInt("y", &data.y, 0, height - 1);
		ImGui::Text("%.3f\n", animTimer);

		animTimer = fmodf(animTimer + ImGui::GetIO().DeltaTime, totalTime);
		auto pct = int((animTimer / totalTime) * height * width);
		data.x = pct % width;
		data.y = (pct - data.x) / height;

// 		auto capture = Textures::GetMaterial(4);
// 		//auto irradiance = Textures::GetMaterial(5);
// 		auto prefilter = Textures::GetMaterial(6);
// 		auto stone = Textures::GetMaterial(1);
// 		ImGui::Image((GLvoid*) (GLuint64) capture->textures[Material::MAP_ENVMAP]->textureID, 
// 					 { 128, 128 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
// 		ImGui::Image((GLvoid*) (GLuint64) prefilter->textures[Material::MAP_DIFFUSE]->textureID, 
// 					 { 128, 128 }, { 0, 0 }, { 1, 1 }, { 1, 1, 0, 1 });
// 		ImGui::Image((GLvoid*) (GLuint64) stone->textures[Material::MAP_DIFFUSE]->textureID, 
// 					 { 128, 128 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });

// 		if (ImGui::CollapsingHeader("Terrain"))
// 		{
// 			EDITVALUEN(World::terrainScale, "Terrain Scale");
// 
// 			ImGui::Checkbox("Draw Grid Lines", &World::drawGridLines);
// 			ImGui::SliderFloat("Grid Line Width", &World::gridLineWidth, 0.f, 1.f);
// 
// 			ImGui::PushItemWidth(200);
// 			ImGui::InputInt("Height Seed", &World::heightSeed);
// 			ImGui::PopItemWidth();
// 			ImGui::SameLine();
// 			if (ImGui::Button("Randomise"))
// 			{
// 				World::RandomiseSeed();
// 			}
// 			ImGui::PushItemWidth(200);
// 			ImGui::InputInt("Chunk Size", &World::chunkSize, World::chunkSize, World::chunkSize);
// 			ImGui::PopItemWidth();
// 
// 			if (ImGui::Button("Regenerate World"))
// 			{
// 				World::GenerateWorld(1);
// 			}
// 			if (ImGui::TreeNode("Height Map"))
// 			{
// 				// Show Heightmap
// 				auto heightmap = Textures::GetMaterial(0);
// 				ImGui::Image((GLvoid*)(GLuint64) heightmap->textures[Material::MAP_HEIGHT]->textureID,
// 							 { 128, 128 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
// 
// 				ImGui::TreePop();
// 			}
// 			ImGui::Separator();
// 
// 			EDITVALUEN(data.wireframe, "Wireframe");
// 			//EDITVALUEN(data.drawArrays, "Draw Arrays");
// 			EDITVALUEN(data.heightFactor, "Height Factor");
// 		}
	}
}