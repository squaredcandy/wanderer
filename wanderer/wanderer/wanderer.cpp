#include "Mesh.h"
#include "Engine.h"
#include "Scene.h"
#include "Render.h"
#include "Entities.h"
#include "Shader.h"

#include <vld.h>
#include<iostream>

const BufferID	BUFFER_FRAME	= 00;
const BufferID	BUFFER_MSAA		= 01;

const MeshID	MESH_SPHERE		= 00;
const MeshID	MESH_PLANE		= 01;

const ShaderID	SHADER_LIGHT	= 00;

namespace Wanderer::Game
{
	using namespace Engine;

	void Init()
	{
		Shaders::CreateShader(SHADER_LIGHT, "light");

		Render::AddBuffer(BUFFER_FRAME);
		Render::InitFrameBuffer(BUFFER_FRAME);
		Render::InitColorBuffer(BUFFER_FRAME, GL_TEXTURE_2D);
		Render::InitDepthBuffer(BUFFER_FRAME);

		Render::AddBuffer(BUFFER_MSAA);
		Render::InitFrameBuffer(BUFFER_MSAA);
		Render::InitColorBuffer(BUFFER_MSAA, GL_TEXTURE_2D_MULTISAMPLE);
		Render::InitDepthBuffer(BUFFER_MSAA);

		Render::drawBufferID = BUFFER_FRAME;
		Render::msaaBufferID = BUFFER_MSAA;

		auto player = Entities::AddEntity("Gary");
		Entities::playerID = player->GetID();

		//Meshes::LoadModel(MESH_SPHERE, "sphere_export.fbx");
		Meshes::LoadModel(MESH_PLANE, "plane_export.fbx");
	}
}

using namespace Wanderer;

int main(int argc, char ** argv)
{
	Engine::initFunction = Game::Init;
	Engine::sceneFunction = Game::Scene::GameScene;
	Engine::tickFunction = Game::Scene::tick;

	Engine::InitializeEngine();
	Engine::GameLoop();
	Engine::Cleanup();

	return 0;
}