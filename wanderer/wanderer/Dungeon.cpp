#include "Dungeon.h"

#define MODEL_FUNC_ARGS int& x, int& y, DungeonStruct& lvl, std::vector<glm::mat4>& models

namespace Wanderer::Engine::Dungeon
{
	float gateSpeed = 15.f;
	float doorMaxHeight = 7.f;

	DungeonID last;
	DungeonID current;
	std::map<DungeonID ,DungeonStruct> dMap;

	DungeonID GenerateNewID()
	{
		static DungeonID lastID = 1;
		return lastID++;
	}

	DungeonStruct& GetCurrentLevel()
	{
		return dMap[current];
	}

	Tile GetTile(int x, int y)
	{
		if (x < 0 || y < 0 || x >= dMap[current].width ||
			y >= dMap[current].height)
			return Tile::Unused;

		return dMap[current].tiles[x + y * dMap[current].width];
	}

	// Checks
	namespace
	{
		bool IsFloorTile(int x, int y)
		{
			auto tile = GetTile(x, y);
			return (tile == Tile::Floor ||
					//tile == Tile::DownStairs ||
					tile == Tile::UpStairs ||
					tile == Tile::OpenDoor ||
					tile == Tile::ClosedDoor);
		}

		bool IsWallTile(int x, int y)
		{
			auto tile = GetTile(x, y);
			return (tile == Tile::Wall);
		}

		bool IsDoorTile(int x, int y)
		{
			auto tile = GetTile(x, y);
			return (tile == Tile::ClosedDoor ||
					tile == Tile::OpeningDoor ||
					tile == Tile::OpenDoor);
		}
	}

	// Updates
	namespace
	{
		using UpdateFunc = std::function<void(UPDATE_FUNC_ARGS)>;

		void UpdateBuffer(GLuint vbo, GLintptr offset, GLsizeiptr size, GLuint mapType,
						  UpdateFunc updateFunc)
		{
			void * data = glMapNamedBufferRange(vbo, offset * size, size, mapType);
			updateFunc(vbo, data, offset, size);
		}

		void UpdateGates()
		{
			auto& lvl = Dungeon::GetCurrentLevel();

			int gateOffset = 0;
			for (auto & gateTile : lvl.gateTiles)
			{
				if (*gateTile == Tile::OpeningDoor)
				{
					auto gateVBO = lvl.models[TileID::Gate].ID;
					auto gateUpdate = [=] (UPDATE_FUNC_ARGS)
					{
						auto mData = *(glm::mat4*) data;
						mData = glm::translate(mData, glm::vec3(0, 0, ImGui::GetIO().DeltaTime * gateSpeed));
						// Check if the height of the translation of the matrix is above the max
						if (glm::vec3(mData[3]).y >= doorMaxHeight)
						{
							*gateTile = Tile::OpenDoor;
						}
						glUnmapNamedBuffer(vbo);
						glNamedBufferSubData(vbo, offset * size, size, &mData);
					};

					UpdateBuffer(gateVBO, gateOffset, sizeof(glm::mat4), GL_MAP_WRITE_BIT, gateUpdate);
				}
				gateOffset++;
			}
		}
	}

	void Tick()
	{
		UpdateGates();
	}

	Mesh * GetModel(TileID modelID)
	{
		return Dungeon::GetCurrentLevel().models[modelID].mesh;
	}

	using ModelFunc = std::function<void(MODEL_FUNC_ARGS)>;
	void AddInstances(TileID tileID, ModelFunc modelFunction, GLuint draw)
	{
		GLuint vbo;
		glm::mat4 model;
		std::vector<glm::mat4> models;
		auto& lvl = Dungeon::GetCurrentLevel();
		Mesh * mesh = GetModel(tileID);
		
		if (mesh == nullptr)
		{
			std::cout << (int) tileID << " not set\n";
			return;
		}

		for (int y = 0; y < lvl.height; ++y)
		{
			for (int x = 0; x < lvl.width; ++x)
			{
				modelFunction(x, y, lvl, models);
			}
		}

		if (models.empty())
		{
			return;
		}

		GLuint64 vec4Size = sizeof(glm::vec4);
		GLuint mat4Size = sizeof(glm::mat4);
		Meshes::CreateVBO(mesh->VAO, vbo, models.size() * mat4Size, &models[0], draw);
		Meshes::AddInstancedAttribute(mesh->VAO, vbo, 5, 4, mat4Size, 0);
		Meshes::AddInstancedAttribute(mesh->VAO, vbo, 6, 4, mat4Size, vec4Size);
		Meshes::AddInstancedAttribute(mesh->VAO, vbo, 7, 4, mat4Size, vec4Size * 2);
		Meshes::AddInstancedAttribute(mesh->VAO, vbo, 8, 4, mat4Size, vec4Size * 3);

		lvl.models[tileID].mesh = mesh;
		lvl.models[tileID].ID = vbo;
		lvl.models[tileID].length = (GLuint) models.size();
	}

	glm::mat4 InitialModelTranslation(glm::mat4 model, int& x, int& y, 
									  int& height, int& width, float heightOffset = 0)
	{
		return glm::translate(model,
			glm::vec3((y * -10) + (height * 5), heightOffset, (x * 10) - (width * 5)));
	}

	void GenerateInstances()
	{
		auto& lvl = GetCurrentLevel();
		for (auto& model : lvl.models)
		{
			instanceFunctionMap[model.first]();
		}
	}

	void DeleteInstances()
	{
		auto& lvl = GetCurrentLevel();
		for (auto& model : lvl.models)
		{
			glDeleteBuffers(1, &model.second.ID);
			model.second.length = 0;
		}
		lvl.gateTiles.clear();
	}

	void SetMesh(TileID tile, Mesh * mesh, int level)
	{
		DungeonStruct& lvl = level == -1 ? GetCurrentLevel() : dMap[level];
		lvl.models[tile].mesh = mesh;
	}

	void AddArchInstances()
	{
		auto modelFunction = [] (MODEL_FUNC_ARGS)
		{
			auto tile = GetTile(x, y);
			if (tile == Tile::ClosedDoor || tile == Tile::OpenDoor)
			{
				auto model = InitialModelTranslation(glm::mat4(), x, y, lvl.height, lvl.width);
				if (IsWallTile(x + 1, y) && IsWallTile(x - 1, y))
				{
					model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1, 0, 0));
				}
				else
				{
					model = glm::rotate(model, glm::radians(90.f), glm::vec3(0, 0, 1));
					model = glm::rotate(model, glm::radians(90.f), glm::vec3(0, 1, 0));
				}
				models.emplace_back(model);
			}
		};
		AddInstances(TileID::Arch, modelFunction, GL_STATIC_DRAW);
	}

	void AddArchCapInstances()
	{
		auto modelFunction = [] (MODEL_FUNC_ARGS)
		{
			auto tile = GetTile(x, y);
			if (tile == Tile::ClosedDoor || tile == Tile::OpenDoor)
			{
				auto model = InitialModelTranslation(glm::mat4(), x, y, lvl.height, lvl.width, 7.f);
				model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1, 0, 0));
				if (!(IsWallTile(x + 1, y) && IsWallTile(x - 1, y)))
				{
					model = glm::rotate(model, glm::radians(-90.f), glm::vec3(0, 0, 1));
				}
				models.emplace_back(model);
			}
		};
		AddInstances(TileID::ArchCap, modelFunction, GL_STATIC_DRAW);
	}

	void AddGateInstances()
	{
		auto modelFunction = [] (MODEL_FUNC_ARGS)
		{
			auto open = -1;
			auto tile = GetTile(x, y);
			if (tile == Tile::ClosedDoor) open = 0;
			else if (tile == Tile::OpenDoor) open = 1;
			// Safety check so that we don't lose the door
			else if (tile == Tile::OpeningDoor)
			{
				tile = Tile::OpenDoor;
				open = 1;
			}
			if (open != -1)
			{
				auto model = InitialModelTranslation(glm::mat4(), x, y, lvl.height, lvl.width,
					(open) ? doorMaxHeight : 0.f);
				model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1, 0, 0));
				if (!(IsWallTile(x + 1, y) && IsWallTile(x - 1, y)))
				{
					model = glm::rotate(model, glm::radians(-90.f), glm::vec3(0, 0, 1));
				}
				// Add the gates to another vector so we can check when they open and dont have iterator over
				// the intial tile vector which can get very expensive
				lvl.gateTiles.emplace_back(&dMap[current].tiles[x + y * dMap[current].width]);
				models.emplace_back(model);
			}
		};
		AddInstances(TileID::Gate, modelFunction, GL_DYNAMIC_DRAW);
	}

	void AddFloorInstances()
	{
		auto modelFunction = [] (MODEL_FUNC_ARGS)
		{
			if (IsFloorTile(x, y))
			{
				auto model = InitialModelTranslation(glm::mat4(), x, y, lvl.height, lvl.width);
				model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1, 0, 0));
				models.emplace_back(model);
			}
		};
		AddInstances(TileID::Floor, modelFunction, GL_STATIC_DRAW);
	}

	void AddWallInstances()
	{
		auto modelFunction = [] (MODEL_FUNC_ARGS)
		{
			if(GetTile(x, y) == Tile::Wall)
			{
				auto model = InitialModelTranslation(glm::mat4(), x, y, lvl.height, lvl.width, 5.f);
				// Check North
				if (IsFloorTile(x, y - 1))
				{
					auto nModel = glm::translate(model, glm::vec3(5.f, 0, 0));
					nModel = glm::rotate(nModel, glm::radians(90.f), glm::vec3(0, 1, 0));
					models.emplace_back(nModel);
				}
				// Check East
				if (IsFloorTile(x + 1, y))
				{
					auto eModel = glm::translate(model, glm::vec3(0, 0, 5.f));
					models.emplace_back(eModel);
				}
				// Check South
				if (IsFloorTile(x, y + 1))
				{
					auto sModel = glm::translate(model, glm::vec3(-5.f, 0, 0));
					sModel = glm::rotate(sModel, glm::radians(-90.f), glm::vec3(0, 1, 0));
					models.emplace_back(sModel);
				}
				// Check West
				if (IsFloorTile(x - 1, y))
				{
					auto wModel = glm::translate(model, glm::vec3(0, 0, -5.f));
					wModel = glm::rotate(wModel, glm::radians(180.f), glm::vec3(0, 1, 0));
					models.emplace_back(wModel);
				}
			}
		};
		AddInstances(TileID::Wall, modelFunction, GL_STATIC_DRAW);
	}

	void AddDownstairInstances()
	{
		auto modelFunction = [] (MODEL_FUNC_ARGS)
		{
			if (GetTile(x, y) == Tile::DownStairs)
			{
				auto model = InitialModelTranslation(glm::mat4(), x, y, lvl.height, lvl.width, -10.f);
				model = glm::rotate(model, glm::radians(90.f), glm::vec3(1, 0, 0));
				models.emplace_back(model);
			}
		};
		AddInstances(TileID::Downstairs, modelFunction, GL_STATIC_DRAW);
	}

	void AddUpstairInstances()
	{
		auto modelFunction = [] (MODEL_FUNC_ARGS)
		{
			if (GetTile(x,y) == Tile::UpStairs)
			{
				auto model = InitialModelTranslation(glm::mat4(), x, y, lvl.height, lvl.width);
				model = glm::rotate(model, glm::radians(90.f), glm::vec3(1, 0, 0));
				
				// Check North
				if (IsFloorTile(x, y - 1))
				{
					//model = glm::translate(model, glm::vec3(5.f, 0, 0));
					models.emplace_back(model);
				}
				// Check East
				else if (IsFloorTile(x + 1, y))
				{
 					//model = glm::translate(model, glm::vec3(0, 0, 5.f));
					model = glm::rotate(model, glm::radians(90.f), glm::vec3(0, 0, 1));
 					models.emplace_back(model);
				}
				// Check South
				else if (IsFloorTile(x, y + 1))
				{
 					//model = glm::translate(model, glm::vec3(-5.f, 0, 0));
					model = glm::rotate(model, glm::radians(180.f), glm::vec3(0, 0, 1));
 					models.emplace_back(model);
				}
				// Check West
				else if (IsFloorTile(x - 1, y))
				{
 					//model = glm::translate(model, glm::vec3(0, 0, -5.f));
					model = glm::rotate(model, glm::radians(-90.f), glm::vec3(0, 0, 1));
 					models.emplace_back(model);
				}
			}
		};
		AddInstances(TileID::Upstairs, modelFunction, GL_STATIC_DRAW);
	}

	namespace
	{
		void setTile(int x, int y, Tile tile)
		{
			dMap[current].tiles[x + y * dMap[current].width] = tile;
		}

		bool placeObject(Tile tile)
		{
			auto& lvl = dMap[current];
			if (lvl.rooms.empty())
				return false;

			int r = Random::RandomInt((int) lvl.rooms.size()); // choose a random room
			int x = Random::RandomInt(lvl.rooms[r].x + 1, lvl.rooms[r].x + lvl.rooms[r].z - 2);
			int y = Random::RandomInt(lvl.rooms[r].y + 1, lvl.rooms[r].y + lvl.rooms[r].w - 2);

			if (GetTile(x, y) == Tile::Floor)
			{
				setTile(x, y, tile);

				// place one object in one room (optional)
				lvl.rooms.erase(lvl.rooms.begin() + r);

				return true;
			}

			return false;
		}

		glm::ivec2 tileWith3Walls(Tile tile)
		{
			auto& lvl = dMap[current];
			for (int i = 0; i < lvl.height; i++)
			{
				for (int j = 0; j < lvl.width; j++)
				{
					glm::ivec2 idx[4] =
					{
						{ i + 1, j },
						{ i, j + 1 },
						{ i - 1, j },
						{ i, j - 1 },
					};

					int numWalls = 0;
					for (auto & k : idx)
					{
						if (GetTile(k.x, k.y) == Tile::Wall)
						{
							numWalls++;
						}
					}

					if (numWalls >= 3)
					{
						if (GetTile(i, j) == tile)
						{
							return { i, j };
						}
					}
				}
			}
			return { -1, -1 };
		}

		bool placeObject(Tile tile, unsigned int settings, bool useCorridor = false)
		{
			auto& lvl = dMap[current];
			auto& room = (settings |= ThreeWalls || useCorridor) ? lvl.corridors : lvl.rooms;

			if (room.empty())
				return false;

			int r = Random::RandomInt((int) room.size()); // choose a random room
			int x, y;
			if (settings |= ThreeWalls)
			{
				if (Random::RandomBool() || true)
				{
					auto coords = tileWith3Walls((useCorridor) ? Tile::Corridor : Tile::Floor);
					// We failed to find one
					if (coords.x == -1 || coords.y == -1)
					{
						return false;
					}
					x = coords.x;
					y = coords.y;
				}
			}
			else
			{
				x = Random::RandomInt(room[r].x + 1, 
									  room[r].x + room[r].z - 2);
				y = Random::RandomInt(room[r].y + 1, 
									  room[r].y + room[r].w - 2);
			}

			if (GetTile(x, y) == Tile::Floor || GetTile(x, y) == Tile::Corridor)
			{
				setTile(x, y, tile);

				if (settings |= RemoveRoom)
				{
					// place one object in one room (optional)
					room.erase(room.begin() + r);
				}

				return true;
			}

			return false;
		}

		bool placeRect(const glm::ivec4& rect, Tile tile)
		{
			auto& lvl = dMap[current];
			if (rect.x < 1 || rect.y < 1 || rect.x + rect.z > lvl.width - 1 || rect.y + rect.w > lvl.height - 1)
				return false;

			for (int y = rect.y; y < rect.y + rect.w; ++y)
			{
				for (int x = rect.x; x < rect.x + rect.z; ++x)
				{
					if (GetTile(x, y) != Tile::Unused)
						return false; // the area already used
				}
			}

			for (int y = rect.y - 1; y < rect.y + rect.w + 1; ++y)
			{
				for (int x = rect.x - 1; x < rect.x + rect.z + 1; ++x)
				{
					if (x == rect.x - 1 || y == rect.y - 1 || x == rect.x + rect.z || y == rect.y + rect.w)
						setTile(x, y, Tile::Wall);
					else
						setTile(x, y, tile);
				}
			}
			return true;
		}

		bool makeRoom(int x, int y, Direction dir, bool firstRoom = false)
		{
			static const int minRoomSize = 3;
			static const int maxRoomSize = 6;

			glm::ivec4 room;
			room.z = Random::RandomInt(minRoomSize, maxRoomSize);
			room.w = Random::RandomInt(minRoomSize, maxRoomSize);

			if (dir == North)
			{
				room.x = x - room.z / 2;
				room.y = y - room.w;
			}

			else if (dir == South)
			{
				room.x = x - room.z / 2;
				room.y = y + 1;
			}

			else if (dir == West)
			{
				room.x = x - room.z;
				room.y = y - room.w / 2;
			}

			else if (dir == East)
			{
				room.x = x + 1;
				room.y = y - room.w / 2;
			}

			if (placeRect(room, Tile::Floor))
			{
				auto& lvl = dMap[current];
				lvl.rooms.emplace_back(room);

				if (dir != South || firstRoom) // North side
					lvl.exits.emplace_back(glm::ivec4{ room.x, room.y - 1, room.z, 1 });
				if (dir != North || firstRoom) // South side
					lvl.exits.emplace_back(glm::ivec4{ room.x, room.y + room.w, room.z, 1 });
				if (dir != East || firstRoom) // West side
					lvl.exits.emplace_back(glm::ivec4{ room.x - 1, room.y, 1, room.w });
				if (dir != West || firstRoom) // East side
					lvl.exits.emplace_back(glm::ivec4{ room.x + room.z, room.y, 1, room.w });

				return true;
			}

			return false;
		}

		bool makeCorridor(int x, int y, Direction dir)
		{
			static const int minCorridorLength = 3;
			static const int maxCorridorLength = 6;

			glm::ivec4 corridor;
			corridor.x = x;
			corridor.y = y;

			if (Random::RandomBool()) // horizontal corridor
			{
				corridor.z = Random::RandomInt(minCorridorLength, maxCorridorLength);
				corridor.w = 1;

				if (dir == North)
				{
					corridor.y = y - 1;

					if (Random::RandomBool()) // west
						corridor.x = x - corridor.z + 1;
				}

				else if (dir == South)
				{
					corridor.y = y + 1;

					if (Random::RandomBool()) // west
						corridor.x = x - corridor.z + 1;
				}

				else if (dir == West)
					corridor.x = x - corridor.z;

				else if (dir == East)
					corridor.x = x + 1;
			}

			else // vertical corridor
			{
				corridor.z = 1;
				corridor.w = Random::RandomInt(minCorridorLength, maxCorridorLength);

				if (dir == North)
					corridor.y = y - corridor.w;

				else if (dir == South)
					corridor.y = y + 1;

				else if (dir == West)
				{
					corridor.x = x - 1;

					if (Random::RandomBool()) // north
						corridor.y = y - corridor.w + 1;
				}

				else if (dir == East)
				{
					corridor.x = x + 1;

					if (Random::RandomBool()) // north
						corridor.y = y - corridor.w + 1;
				}
			}

			if (placeRect(corridor, Tile::Corridor))
			{
				auto& lvl = dMap[current];
				lvl.corridors.emplace_back(corridor);
				if (dir != South && corridor.z != 1) // north side
					lvl.exits.emplace_back(glm::ivec4{ corridor.x, corridor.y - 1, corridor.z, 1 });
				if (dir != North && corridor.z != 1) // south side
					lvl.exits.emplace_back(glm::ivec4{ corridor.x, corridor.y + corridor.w, corridor.z, 1 });
				if (dir != East && corridor.w != 1) // west side
					lvl.exits.emplace_back(glm::ivec4{ corridor.x - 1, corridor.y, 1, corridor.w });
				if (dir != West && corridor.w != 1) // east side
					lvl.exits.emplace_back(glm::ivec4{ corridor.x + corridor.z, corridor.y, 1, corridor.w });

				return true;
			}

			return false;
		}
	
		bool createFeature(int x, int y, Direction dir)
		{
			static const int roomChance = 50; // corridorChance = 100 - roomChance

			int dx = 0;
			int dy = 0;

			if (dir == North)
				dy = 1;
			else if (dir == South)
				dy = -1;
			else if (dir == West)
				dx = 1;
			else if (dir == East)
				dx = -1;

			if (GetTile(x + dx, y + dy) != Tile::Floor &&
				GetTile(x + dx, y + dy) != Tile::Corridor)
				return false;

			if (Random::RandomInt(100) < roomChance)
			{
				if (makeRoom(x, y, dir))
				{
					setTile(x, y, Tile::ClosedDoor);

					return true;
				}
			}

			else
			{
				if (makeCorridor(x, y, dir))
				{
					if (GetTile(x + dx, y + dy) == Tile::Floor)
						setTile(x, y, Tile::ClosedDoor);
					else // don't place a door between corridors
						setTile(x, y, Tile::Corridor);

					return true;
				}
			}

			return false;
		}

		bool createFeature()
		{
			for (int i = 0; i < 1000; ++i)
			{
				auto& lvl = dMap[current];
				if (lvl.exits.empty())
					break;

				// choose a random side of a random room or corridor
				int r = Random::RandomInt((int) lvl.exits.size());
				int x = Random::RandomInt(lvl.exits[r].x, lvl.exits[r].x + lvl.exits[r].z - 1);
				int y = Random::RandomInt(lvl.exits[r].y, lvl.exits[r].y + lvl.exits[r].w - 1);

				// north, south, west, east
				for (int j = 0; j < Direction::Length; ++j)
				{
					if (createFeature(x, y, static_cast<Direction>(j)))
					{
						lvl.exits.erase(lvl.exits.begin() + r);
						return true;
					}
				}
			}

			return false;
		}

		void trimLevel()
		{
			auto& lvl = dMap[current];
			int newHeight = lvl.height;
			int newWidth = lvl.width;

			// Trim left and right
			for (int x = 0; x < newWidth; ++x)
			{
				bool empty = true;
				for (int y = 0; y < newHeight; ++y)
				{
					if (GetTile(x, y) != Tile::Unused)
					{
						empty = false;
						break;
					}
				}

				if (empty)
				{
					auto start = lvl.tiles.begin() + x;
					for (int y = 0; y < newHeight - 1; ++y)
					{
						start = lvl.tiles.erase(start);
						start += newWidth - 1;
					}
					x--;
					lvl.width = --newWidth;
				}
			}

			// Trim above and below
			for (int x = 0; x < newHeight; ++x)
			{
				bool empty = true;
				for (int y = 0; y < newWidth; ++y)
				{
					if (GetTile(y, x) != Tile::Unused)
					{
						empty = false;
						break;
					}
				}
				if (empty)
				{
					auto start = lvl.tiles.begin() + (x * newWidth);
					lvl.tiles.erase(start, start + newWidth);
					x--;
					lvl.height = --newHeight;
				}
			}
		}
	}

	void SetStairs(DungeonID top, DungeonID bottom)
	{
		try
		{
			auto topLevel = dMap[top];
			auto bottomLevel = dMap[bottom];

			topLevel.down = bottom;
			bottomLevel.up = top;

		}
		catch (const std::exception&)
		{
			std::cout << "Couldn't connect the levels" << top << " and " << bottom << "\n";
		}
	}

	void GenerateDungeonLevel(int width, int height, int maxFeatures, bool hasStairs)
	{
		if (current != UNSET_DUNGEONID)
		{
			DeleteInstances();
		}

		last = current;
		current = GenerateNewID();
		dMap[current] = DungeonStruct(width, height);
		auto& lvl = dMap[current];

		SetStairs(last, current);

		// place the first room in the center
		if (!makeRoom(lvl.width / 2, lvl.height / 2, 
					  static_cast<Direction>(Random::RandomInt(4), true)))
		{
			std::cout << "Unable to place the first room.\n";
			return;
		}

		// we already placed 1 feature (the first room)
		for (int i = 1; i < maxFeatures; ++i)
		{
			if (!createFeature())
			{
				std::cout << "Unable to place more features (placed " << i << ").\n";
				break;
			}
		}

		if (hasStairs)
		{
			if (!placeObject(Tile::UpStairs, ThreeWalls, true))
			{
				std::cout << "Unable to place up stairs.\n";
				return;
			}

			if (!placeObject(Tile::DownStairs))
			{
				std::cout << "Unable to place down stairs.\n";
				return;
			}
		}

		for (auto& tile : lvl.tiles)
		{
			//if (tile == (char) Tile::Unused)
			//	tile = ' ';
			//if (tile == (char) Tile::Floor || tile == (char) Tile::Corridor)
			//	tile = ' ';
			if (tile == Tile::Corridor)
				tile = Tile::Floor;
		}

		trimLevel();
	}

	void LoadLevel(DungeonID levelID)
	{
		try
		{
			dMap[levelID];
		}
		catch (const std::exception&)
		{
			std::cout << "Couldn't load level: " << levelID << std::endl;
			return;
		}

		DeleteInstances();
		last = current;
		current = levelID;
		GenerateInstances();
	}

	void Print()
	{
		auto& lvl = dMap[current];
		std::cout << "Level: " << current << "\n";
		for (int y = 0; y < lvl.height; ++y)
		{
			for (int x = 0; x < lvl.width; ++x)
				std::cout << (char) GetTile(x, y);

			std::cout << std::endl;
		}
	}

	void Cleanup()
	{
		for (auto const&[ID, dStruct] : dMap)
		{
			for (auto const & [str, vbo] : dStruct.models)
			{
				glDeleteBuffers(1, &vbo.ID);
			}
		}
	}
}