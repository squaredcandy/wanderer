#pragma once

#include "Entities.h"
#include "Debug.h"
#include "Font.h"

#include <functional>
#include <memory>
#include <vector>
#include <map>

#include <SDL/SDL.h>
#include <GL/gl3w.h>
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl.h>

namespace Wanderer::Engine::Render
{
	void Cleanup();
	void RenderWorld();
}

namespace Wanderer::Engine::Meshes
{
	void Cleanup();
}

namespace Wanderer::Engine::Shaders
{
	void Cleanup();
}

struct ModelInstance
{

};

namespace Wanderer::Engine
{
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