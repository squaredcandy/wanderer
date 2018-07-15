#pragma once

#include "Debug.h"
#include "Font.h"
#include "Entities.h"

#include <functional>
#include <memory>
#include <vector>
#include <map>

#include <SDL/SDL.h>
#include <GL/gl3w.h>
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl.h>

struct ModelInstance
{

};

namespace Wanderer::Engine
{
	namespace Render
	{
		void Cleanup();
		void RenderWorld();
		void RenderDungeon();
	}
	namespace Meshes
	{
		void Cleanup();
	}
	namespace Shaders
	{
		void Cleanup();
	}
	namespace Textures
	{
		void Cleanup();
	}
	namespace Dungeon
	{
		void Cleanup();
	}

	// Window and Context handlers
	extern SDL_Window * window;
	extern SDL_GLContext context;

	// Background Color
	extern ImVec4 backgroundColor;

	// User Definable functions
	extern std::function<void()> initFunction;
	extern std::function<void()> sceneFunction;
	extern std::function<void()> tickFunction;

	// Do we update this frame?
	extern bool doTick;
	
	void APIENTRY glErrorCallback(GLenum source, GLenum type,
								  GLuint id, GLenum severity,
								  GLsizei length, const GLchar * message,
								  const void * userParam);
	void GLSettings();
	void InitializeEngine();
	void GameLoop();
	void EmptyScene();
	void ClearWindow();
	void DrawWindow();
	void Cleanup();
}