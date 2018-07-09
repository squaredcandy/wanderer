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
	DownStairs	= '>'
};

enum Direction
{
	North,
	South,
	West,
	East,
	Length = 4
};

using DungeonID = unsigned int;

const DungeonID UNSET_DUNGEONID = 0U;

struct DungeonStruct
{
	int width, height;

	// Tiles
	std::vector<Tile> tiles;
	std::vector<Tile*> gateTiles;

	// Rooms and Exits for generation
	std::vector<glm::ivec4> rooms;
	std::vector<glm::ivec4> exits;
	DungeonID up, down;
	
	// VBO to store matrix data
	// VBO ID | model length
	std::map<std::string, std::tuple<GLuint, GLuint>> vbos;

	DungeonStruct() :
		width(0), height(0),
		up(UNSET_DUNGEONID), down(UNSET_DUNGEONID),
		tiles(0, Tile::Unused),
		gateTiles(),
		rooms(),
		exits() {};

	DungeonStruct(int _width, int _height) :
		width(_width), height(_height),
		up(UNSET_DUNGEONID), down(UNSET_DUNGEONID),
		tiles(width * height, Tile::Unused),
		gateTiles(),
		rooms(),
		exits() {};
};

namespace Wanderer::Engine::Dungeon
{
	//extern float gateSpeed;
	extern DungeonID last;
	extern DungeonID current;
	extern std::map<DungeonID, DungeonStruct> dMap;

	const extern std::string archID;
	const extern std::string archCapID;
	const extern std::string gateID;
	const extern std::string floorID;
	const extern std::string wallID;

	DungeonStruct& GetCurrentLevel();
	Tile GetTile(int x, int y);
	
	void Tick();

	void DeleteInstances();

	void AddArchInstances(Mesh& mesh);
	void AddArchCapInstances(Mesh& mesh);
	void AddGateInstances(Mesh& mesh);

	void AddFloorInstances(Mesh& mesh);
	void AddWallInstances(Mesh& mesh);
	
	void GenerateDungeonLevel(int width, int height, int maxFeatures, 
							  bool hasStairs = true);
	void LoadLevel(DungeonID levelID);
	void Print();
	void Cleanup();
}