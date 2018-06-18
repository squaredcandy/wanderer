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
	}
}
