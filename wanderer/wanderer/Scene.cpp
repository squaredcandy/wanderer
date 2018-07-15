#include "Scene.h"

namespace Wanderer::Game::Scene
{
	void MainMenu()
	{
	}

	void GameScene()
	{
		if (Engine::Flag::showDebugGraphWindow) UI::DebugGraphWindow();
		if (Engine::Flag::showDebugWindow) UI::DebugWindow();
		//bool a = true; ImGui::ShowDemoWindow(&a);
	}

	void Tick()
	{
		auto& lvl = Dungeon::GetCurrentLevel();

		if (ImGui::IsKeyPressed(SDL_SCANCODE_G, false))
		{
			auto found = std::find_if(lvl.gateTiles.begin(), lvl.gateTiles.end(), 
									  [] (Tile * tile) 
			{
				return (*tile == Tile::ClosedDoor);
			});

			if (found != lvl.gateTiles.end())
			{
				**found = Tile::OpeningDoor;
			}
		}
		auto dt = ImGui::GetIO().DeltaTime;
		Entities::Update(dt);
		Dungeon::Tick();
	}
}
