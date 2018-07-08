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

	}

	void tick()
	{
		auto lvl = Dungeon::GetCurrentLevel();

		if (ImGui::IsKeyPressed(SDL_SCANCODE_G))
		{
			auto door = std::find(lvl.tiles.begin(), lvl.tiles.end(), Tile::ClosedDoor);
			if (door != lvl.tiles.end())
			{
				*door = Tile::OpeningDoor;
			}
			
		}
		
		auto found = std::find(lvl.tiles.begin(), lvl.tiles.end(), Tile::OpeningDoor);
		if (found != lvl.tiles.end())
		{
			void * data = glMapNamedBufferRange(std::get<0>(lvl.vbos["Gate"]), 0, sizeof(glm::mat4), GL_MAP_WRITE_BIT);

			// attempt to convert to mat4
			auto mData = *(glm::mat4*) data;
			int i = 0;
		}
	}
}
