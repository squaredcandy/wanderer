#include "Mesh.h"
#include "Engine.h"
#include "Scene.h"
#include "Render.h"
//#include "Entities.h"
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
const BufferID	BUFFER_CAPTURE = 03;

namespace Wanderer::Game
{
	using namespace Engine;

	void SetupPBR()
	{
		glFrontFace(GL_CW);
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		std::array<glm::mat4, 6> captureViews = {
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
		};

		Shaders::CreateShader(SHADER_EQUTOCUBE, "equtocube");
		Shaders::CreateShader(SHADER_IRRADIANCE, "irradiance");
		Shaders::CreateShader(SHADER_PREFILTER, "prefilter");
		Shaders::CreateShader(SHADER_BRDF, "brdf");

		Render::AddBuffer(BUFFER_CAPTURE);
		Render::InitFrameBuffer(BUFFER_CAPTURE);
		Render::InitDepthBuffer(BUFFER_CAPTURE);

		auto buffer = Render::GetBuffer(BUFFER_CAPTURE);

		int size = 512;
		int size2 = 32;
		int size3 = 128;

		// Environment Map
		//Render::SetupCaptureBuffer(buffer, size);
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.frame);
		glBindRenderbuffer(GL_RENDERBUFFER, buffer.depth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer.depth);


		Textures::LoadHDREnvMap("Capture", "Footprint_Court_2k.hdr");
		Textures::CreateEmptyCubemap("Capture", size, GL_LINEAR_MIPMAP_LINEAR);

		Render::RenderEnvMap(BUFFER_CAPTURE, size, captureProjection, captureViews);

		// Irradiance Map
		Textures::CreateEmptyCubemap("Irradiance", size2, GL_LINEAR);
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.frame);
		glBindRenderbuffer(GL_RENDERBUFFER, buffer.depth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size2, size2);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glBindRenderbuffer(GL_RENDERBUFFER, 0);

		Render::RenderIrradianceMap(BUFFER_CAPTURE, size2, captureProjection, captureViews);

		// Pre-filter
		Textures::CreateEmptyCubemap("Prefilter", size3, GL_LINEAR_MIPMAP_LINEAR, true);
		Render::RenderPrefilterMap(BUFFER_CAPTURE, size3, captureProjection, captureViews);

		// BRDF LUT
		Textures::CreateEmptyMaterial("Prefilter", Material::MAP_DIFFUSE, GL_RG, 512);
		buffer = Render::GetBuffer(BUFFER_CAPTURE);
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.frame);
		glBindRenderbuffer(GL_RENDERBUFFER, buffer.depth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Textures::GetMaterial("Prefilter")->textures[Material::MAP_DIFFUSE].textureID, 0);

		Render::RenderBRDF(size);
		glFrontFace(GL_CCW);
	}

	void SetupShadows()
	{
		int size = 1024;
		auto& buffer = Render::GetBuffer(BUFFER_SHADOW);
		Render::AddBuffer(BUFFER_SHADOW);
		Render::InitFrameBuffer(BUFFER_SHADOW);
		Render::InitColorBuffer(BUFFER_SHADOW, GL_TEXTURE_CUBE_MAP);

		auto mat = Textures::CreateMaterial("Shadow");
		auto cubemap = Texture();
		glGenTextures(1, &cubemap.textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.textureID);
		for (unsigned int i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindFramebuffer(GL_FRAMEBUFFER, buffer.frame);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubemap.textureID, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		mat->textures[Material::MAP_CUBEMAP] = cubemap;
	}

	void Init()
	{
		Reflection::AddDefaultReflects();

		// Shader
		//Shaders::CreateShader(SHADER_NORMAL, "normal", true, true, true, true);
		//Shaders::CreateShader(SHADER_TERRAIN, "terrain", true, true, false, true);
		//Shaders::CreateShader(SHADER_SHADOW, "shadow", true, true, false, true);
		Shaders::CreateShader(SHADER_SHADOW, "pshadow", true, true, true);
		Shaders::CreateShader(SHADER_BASIC, "basic");
		//Shaders::CreateShader(SHADER_DUNGEON, "dungeon");
		Shaders::CreateShader(SHADER_PBR, "pbr");
		Shaders::CreateShader(SHADER_SPRITE, "sprite");

		Shaders::SetCurrentShader(SHADER_PBR);

// 		Shaders::SetInt("irradianceMap", 0);
// 		Shaders::SetInt("prefilterMap", 1);
// 		Shaders::SetInt("brdfLUT", 2);
		Shaders::SetInt("albedoMap", 3);
		Shaders::SetInt("normalMap", 4);
		Shaders::SetInt("metallicMap", 5);
		Shaders::SetInt("roughnessMap", 6);
		Shaders::SetInt("aoMap", 7);
		Shaders::SetInt("heightMap", 8);
		const int S = 9;
		for (int i = 0; i < 8; ++i)
		{
			Shaders::SetInt("shadowMap[" + std::to_string(i) + "]", S + i);
		}

		// Render Buffers
		Render::AddBuffer(BUFFER_FRAME);
		Render::InitFrameBuffer(BUFFER_FRAME);
		Render::InitColorBuffer(BUFFER_FRAME, GL_TEXTURE_2D);
		Render::InitDepthBuffer(BUFFER_FRAME);

		Render::AddBuffer(BUFFER_MSAA);
		Render::InitFrameBuffer(BUFFER_MSAA);
		Render::InitColorBuffer(BUFFER_MSAA, GL_TEXTURE_2D_MULTISAMPLE);
		Render::InitDepthBuffer(BUFFER_MSAA);

		// Setup Shadows
		//SetupShadows();

		// Setup our PBR Reflection
		// PBR Reflection under works
		
		Render::drawBufferID = BUFFER_FRAME;
		Render::msaaBufferID = BUFFER_MSAA;
		//Render::shdwBufferID = BUFFER_SHADOW;

		// World
		//World::RandomiseSeed();
		//World::GenerateWorld(CHUNK_LENGTH);

		// Camera
		Camera::AddCamera(CAMERA_PLAYER);

		// Mesh
		//auto random = Meshes::LoadModel(100U, "random.fbx");
		auto plane = Meshes::LoadModel(MESH_SPRITE, "plane_export.fbx");
		auto lightSphere = Meshes::LoadModel(MESH_SPHERE, "sphere_export.fbx");
		auto doorArch = Meshes::LoadModel(MESH_DOORARCH, "arch_export.fbx");
		auto doorCap = Meshes::LoadModel(MESH_DOORCAP, "arch_cap_export.fbx");
		auto gate = Meshes::LoadModel(MESH_GATE, "gate_export.fbx");
		auto floor = Meshes::LoadModel(MESH_FLOOR, "plane_export.fbx");
		auto wall = Meshes::LoadModel(MESH_WALL, "plane_export.fbx");
		auto upstairs = Meshes::LoadModel(MESH_UPSTAIR, "upstairs_export.fbx");
		auto downstairs = Meshes::LoadModel(MESH_DOWNSTAIR, "upstairs_export.fbx");

		// Entity
		auto player = Entities::AddEntity("Gary");
		Entities::playerID = player->GetID();

		auto light0 = Entities::AddEntity("Light0");
		light0->AddMod<ModLight>();
		//light0->AddMod<ModLightMod>();

// 		auto light1 = Entities::AddEntity("Light1");
// 		light1->AddMod<ModLight>().GetColor() = glm::vec3(500);
// 
// 		auto light2 = Entities::AddEntity("Light0");
// 		light2->AddMod<ModLight>().GetColor() = glm::vec3(500);

		// Texture
		Textures::CreateMaterial("Stonewall");
		Textures::LoadStaticMaterial("Stonewall", "StoneWall_1024_albedo.png", 
									 Material::MAP_DIFFUSE);
		Textures::LoadStaticMaterial("Stonewall", "StoneWall_1024_normal.png",
									 Material::MAP_NORMAL);
		Textures::LoadStaticMaterial("Stonewall", "StoneWall_1024_metallic.png",
									 Material::MAP_METALLIC);
		Textures::LoadStaticMaterial("Stonewall", "StoneWall_1024_roughness.png",
									 Material::MAP_ROUGHNESS);
		Textures::LoadStaticMaterial("Stonewall", "StoneWall_1024_ao.png",
									 Material::MAP_AO);
		Textures::LoadStaticMaterial("Stonewall", "StoneWall_1024_height.png",
									 Material::MAP_HEIGHT);

		Textures::CreateMaterial("Cobble2");
		Textures::LoadStaticMaterial("Cobble2", "MedievalCobblestone2_1024_albedo.png",
									 Material::MAP_DIFFUSE);
		Textures::LoadStaticMaterial("Cobble2", "MedievalCobblestone2_1024_normal.png",
									 Material::MAP_NORMAL);
		Textures::LoadStaticMaterial("Cobble2", "MedievalCobblestone2_1024_metallic.png",
									 Material::MAP_METALLIC);
		Textures::LoadStaticMaterial("Cobble2", "MedievalCobblestone2_1024_roughness.png",
									 Material::MAP_ROUGHNESS);
		Textures::LoadStaticMaterial("Cobble2", "MedievalCobblestone2_1024_ao.png",
									 Material::MAP_AO);
		Textures::LoadStaticMaterial("Cobble2", "MedievalCobblestone2_1024_height.png",
									 Material::MAP_HEIGHT);

// 		Textures::CreateMaterial(TEX_STEELRUST);
// 		Textures::LoadStaticMaterial(TEX_STEELRUST, "SteelRusted_1K_albedo.png",
// 									 Material::MAP_DIFFUSE);
// 		Textures::LoadStaticMaterial(TEX_STEELRUST, "SteelRusted_1K_normal.png",
// 									 Material::MAP_NORMAL);
// 		Textures::LoadStaticMaterial(TEX_STEELRUST, "SteelRusted_1K_metallic.png",
// 									 Material::MAP_METALLIC);
// 		Textures::LoadStaticMaterial(TEX_STEELRUST, "SteelRusted_1K_roughness.png",
// 									 Material::MAP_ROUGHNESS);
// 		Textures::LoadStaticMaterial(TEX_STEELRUST, "SteelRusted_1K_ao.png",
// 									 Material::MAP_AO);

		Textures::CreateMaterial("SteelBattered");
		Textures::LoadStaticMaterial("SteelBattered", "SteelBattered_1K_albedo.png",
									 Material::MAP_DIFFUSE);
		Textures::LoadStaticMaterial("SteelBattered", "SteelBattered_1K_normal.png",
									 Material::MAP_NORMAL);
		Textures::LoadStaticMaterial("SteelBattered", "SteelBattered_1K_metallic.png",
									 Material::MAP_METALLIC);
		Textures::LoadStaticMaterial("SteelBattered", "SteelBattered_1K_roughness.png",
									 Material::MAP_ROUGHNESS);
		Textures::LoadStaticMaterial("SteelBattered", "SteelBattered_1K_ao.png",
									 Material::MAP_AO);
		Textures::LoadStaticMaterial("SteelBattered", "SteelBattered_1K_height.png",
									 Material::MAP_HEIGHT);

		Textures::CreateMaterial("Slash");
		Textures::LoadStaticMaterial("Slash", "slash.png", Material::MAP_DIFFUSE);

		// Dungeon
		int len = 100;
		int features = 30;
		Dungeon::GenerateDungeonLevel(len, len, features);
		Dungeon::SetMesh(TileID::Arch, doorArch);
		Dungeon::SetMesh(TileID::ArchCap, doorCap);
		Dungeon::SetMesh(TileID::Gate, gate);
		Dungeon::SetMesh(TileID::Floor, floor);
		Dungeon::SetMesh(TileID::Wall, wall);
		Dungeon::SetMesh(TileID::Upstairs, upstairs);
		Dungeon::SetMesh(TileID::Downstairs, downstairs);
		Dungeon::GenerateInstances();
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