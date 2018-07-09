#include "Mesh.h"
#include "Engine.h"
#include "Scene.h"
#include "Render.h"
#include "Entities.h"
#include "Shader.h"
#include "World.h"
#include "Camera.h"
#include "Dungeon.h"
#include "Texture.h"
//#include "Reflection.h"

#include <vld.h>
#include <iostream>

const BufferID	BUFFER_FRAME = 00;
const BufferID	BUFFER_MSAA = 01;
const BufferID	BUFFER_SHADOW = 02;

namespace Wanderer::Game
{
	using namespace Engine;

	void Init()
	{
		Reflection::AddDefaultReflects();

		// Shader
		//Shaders::CreateShader(SHADER_NORMAL, "normal", true, true, true, true);
		//Shaders::CreateShader(SHADER_TERRAIN, "terrain", true, true, false, true);
		Shaders::CreateShader(SHADER_BASIC, "basic");
		//Shaders::CreateShader(SHADER_SHADOW, "shadow", true, true, false, true);
		Shaders::CreateShader(SHADER_DUNGEON, "dungeon");

		// Render Buffers
		Render::AddBuffer(BUFFER_FRAME);
		Render::InitFrameBuffer(BUFFER_FRAME);
		Render::InitColorBuffer(BUFFER_FRAME, GL_TEXTURE_2D);
		Render::InitDepthBuffer(BUFFER_FRAME);

		Render::AddBuffer(BUFFER_MSAA);
		Render::InitFrameBuffer(BUFFER_MSAA);
		Render::InitColorBuffer(BUFFER_MSAA, GL_TEXTURE_2D_MULTISAMPLE);
		Render::InitDepthBuffer(BUFFER_MSAA);

		Render::AddBuffer(BUFFER_SHADOW);
		Render::InitFrameBuffer(BUFFER_SHADOW);
		Render::InitColorBuffer(BUFFER_SHADOW, GL_TEXTURE_2D);

		Render::drawBufferID = BUFFER_FRAME;
		Render::msaaBufferID = BUFFER_MSAA;
		Render::shdwBufferID = BUFFER_SHADOW;

		// World
		World::RandomiseSeed();
		World::GenerateWorld(CHUNK_LENGTH);

		// Camera
		Camera::AddCamera(CAMERA_PLAYER);

		// Mesh
		auto plane = Meshes::LoadModel(MESH_PLANE, "plane_export.fbx");
		auto wall = Meshes::LoadModel(MESH_WALL, "plane_export.fbx");
		auto doorArch = Meshes::LoadModel(MESH_DOORARCH, "arch_export.fbx");
		auto doorCap = Meshes::LoadModel(MESH_DOORCAP, "arch_cap_export.fbx");
		auto gate = Meshes::LoadModel(MESH_GATE, "gate_export.fbx");

		// Entity
		auto player = Entities::AddEntity("Gary");
		Entities::playerID = player->GetID();

		// Texture
		Textures::CreateMaterial(TEX_STONEWALL);
		Textures::LoadStaticMaterial(TEX_STONEWALL, 
									 "StoneWall_1024_albedo.png", 
									 Material::MAP_DIFFUSE);

		Textures::CreateMaterial(TEX_COBBLE);
		Textures::LoadStaticMaterial(TEX_COBBLE, 
									 "MedievalCobblestone2_1024_albedo.png",
									 Material::MAP_DIFFUSE);

		Textures::CreateMaterial(TEX_ARCH);
		Textures::LoadStaticMaterial(TEX_ARCH,
									 "arch_uv.png",
									 Material::MAP_DIFFUSE);

		Textures::CreateMaterial(TEX_STEELRUST);
		Textures::LoadStaticMaterial(TEX_STEELRUST,
									 "SteelRusted_1K_albedo.png",
									 Material::MAP_DIFFUSE);

		// Dungeon
		Dungeon::GenerateDungeonLevel(30, 30, 6);
		Dungeon::Print();

		// Change this to set a mesh in dungeon.cpp and them work off them
		// instead of actually passing in a reference everytime
		// Put it in DungeonStruct so we can have different sets for different levels
		// Maybe put the material in there as well so we can automate the rendering process
		// Add a boolean in there or check for nullptr for checking if we need to draw something or not

		Dungeon::AddFloorInstances(*plane);
		Dungeon::AddWallInstances(*wall);
		
		Dungeon::AddArchInstances(*doorArch);
		Dungeon::AddArchCapInstances(*doorCap);
		Dungeon::AddGateInstances(*gate);

		Dungeon::DeleteInstances();

		Dungeon::GenerateDungeonLevel(30, 30, 6);
		Dungeon::Print();

		Dungeon::AddFloorInstances(*plane);
		Dungeon::AddWallInstances(*wall);

		Dungeon::AddArchInstances(*doorArch);
		Dungeon::AddArchCapInstances(*doorCap);
		Dungeon::AddGateInstances(*gate);

		Dungeon::DeleteInstances();

		Dungeon::LoadLevel(1);

		Dungeon::AddFloorInstances(*plane);
		Dungeon::AddWallInstances(*wall);

		Dungeon::AddArchInstances(*doorArch);
		Dungeon::AddArchCapInstances(*doorCap);
		Dungeon::AddGateInstances(*gate);
	}
}

using namespace Wanderer;

int main(int argc, char ** argv)
{
	Engine::initFunction = Game::Init;
	Engine::sceneFunction = Game::Scene::GameScene;
	Engine::tickFunction = Game::Scene::Tick;

	Engine::InitializeEngine();
	Engine::GameLoop();
	Engine::Cleanup();

	return 0;
}