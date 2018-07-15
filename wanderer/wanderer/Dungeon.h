#pragma once

#include "Random.h"
#include "Mesh.h"

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <tuple>
#include <functional>

#include <ImGui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>

#define UPDATE_FUNC_ARGS GLuint vbo, void * data, GLintptr offset, GLsizeiptr size

enum TileSettings
{
	RemoveRoom = 0x1,
	ThreeWalls = 0x2,
};

enum class Tile
{
	Unused		= ' ',
	Floor		= '.',
	Corridor	= ',',
	Wall		= '#',
	ClosedDoor	= '+',
	OpeningDoor = '=',
	OpenDoor	= '-',
	UpStairs	= '<',
	DownStairs	= '>',
	ClosedChest	= '[',
	OpenChest	= ']'
};

enum class TileID
{
	Arch,
	ArchCap,
	Gate,
	Floor,
	Wall,
	Upstairs,
	Downstairs
};

enum Direction
{
	North,
	South,
	West,
	East,
	Length = 4
};

namespace Wanderer::Engine::Dungeon
{
	void AddArchInstances();
	void AddArchCapInstances();
	void AddGateInstances();
	void AddFloorInstances();
	void AddWallInstances();
	void AddUpstairInstances();
	void AddDownstairInstances();
}

using namespace Wanderer::Engine;
static std::map<TileID, std::function<void()>> instanceFunctionMap
{
	{ std::make_pair(TileID::Arch,		Dungeon::AddArchInstances) },
	{ std::make_pair(TileID::ArchCap,	Dungeon::AddArchCapInstances) },
	{ std::make_pair(TileID::Gate,		Dungeon::AddGateInstances) },
	{ std::make_pair(TileID::Floor,		Dungeon::AddFloorInstances) },
	{ std::make_pair(TileID::Wall,		Dungeon::AddWallInstances) },
	{ std::make_pair(TileID::Upstairs,	Dungeon::AddUpstairInstances) },
	{ std::make_pair(TileID::Downstairs, Dungeon::AddDownstairInstances) }
};

using DungeonID = unsigned int;

const DungeonID UNSET_DUNGEONID = 0U;

struct DungeonModelData
{
	GLuint ID;
	GLuint length;

	Mesh * mesh;

	// Implement this so we can streamline material assignment
	//std::map<int, Material*> materials;

	DungeonModelData() :
		ID(), length(), mesh()/*, materials() */{}
};

struct DungeonStruct
{
	int width, height;

	// Tiles
	std::vector<Tile> tiles;
	std::vector<Tile*> gateTiles;

	// Rooms and Exits for generation
	std::vector<glm::ivec4> rooms;
	std::vector<glm::ivec4> corridors;
	std::vector<glm::ivec4> exits;
	DungeonID up, down;
	
	// VBO to store matrix data
	// VBO ID | model length
	std::map<TileID, DungeonModelData> models;

	void InitModels()
	{
		for (auto& id : instanceFunctionMap)
		{
			models[id.first] = DungeonModelData();
		}
	}

	DungeonStruct() :
		width(0), height(0),
		up(UNSET_DUNGEONID), down(UNSET_DUNGEONID),
		tiles(0, Tile::Unused),
		gateTiles(),
		rooms(),
		exits() 
	{
		InitModels();
	};

	DungeonStruct(int _width, int _height) :
		width(_width), height(_height),
		up(UNSET_DUNGEONID), down(UNSET_DUNGEONID),
		tiles(width * height, Tile::Unused),
		gateTiles(),
		rooms(),
		exits() 
	{
		InitModels();
	};
};

namespace Wanderer::Engine::Dungeon
{
	//extern float gateSpeed;
	extern DungeonID last;
	extern DungeonID current;
	extern std::map<DungeonID, DungeonStruct> dMap;

	DungeonStruct& GetCurrentLevel();
	Tile GetTile(int x, int y);
	
	void Tick();

	void GenerateInstances();
	void DeleteInstances();

	void SetMesh(TileID tile, Mesh * mesh, int level = -1);
	
	void GenerateDungeonLevel(int width, int height, int maxFeatures, 
							  bool hasStairs = true);
	void LoadLevel(DungeonID levelID);
	void Print();
	void Cleanup();
}