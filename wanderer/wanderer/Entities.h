#pragma once

#include "Entity.h"

#include "Mods.h"

//#include <optional>

namespace Wanderer::Engine::Entities
{
	extern EntityID playerID;

	void Update(float deltaTime);
	void RemoveInactive();
	void RemoveAll();
	std::size_t Size();
	Entity * AddEntity(std::string name = "",
					   glm::vec3 translate = glm::vec3(5, 0, 5),
					   glm::vec3 rotate = glm::vec3(),
					   glm::vec3 scale = glm::vec3(1, 1, 1));
	Entity * Get(EntityID id) noexcept;
}

//class Entities
//{
//private:
//	
//
//public:
//
//
//
//
//
////	int getEntity(EntityID id)
//	{
//		for (auto i = 0; i < entities.size(); ++i)
//		{
//			if (entities[i]->getID() == id)
//			{
//				return i;
//			}
//		}
//		return -1;
//	}
//
//	int getEntity(std::string name)
//	{
//		for (auto i = 0; i < entities.size(); ++i)
//		{
//			if (Stricmp(entities[i]->getCommon().Name().c_str(), name.c_str()) == 0)
//			{
//				return i;
//			}
//		}
//		return -1;
//	}
//
//	// Get Indexes
//	std::vector<int> getEntities(std::string name)
//	{
//		std::vector<int> found;
//		for (auto i = 0; i < entities.size(); ++i)
//		{
//			if (Stricmp(entities[i]->getCommon().Name().c_str(), name.c_str()) == 0)
//			{
//				found.emplace_back(i);
//			}
//		}
//		return found;
//	}
//
//	// Get Index
//	int getClosestEntity(int cmpIndex, std::vector<int> otherIndex)
//	{
//		float bestDistance = FLT_MAX;
//		int idx = -1;
//		auto cmpPos = entities[cmpIndex]->getTransform().Translation();
//		for (int i : otherIndex)
//		{
//
//			float dist = glm::distance(
//				cmpPos, entities[i]->getTransform().Translation());
//			if (dist < bestDistance)
//			{
//				bestDistance = dist;
//				idx = i;
//			}
//		}
//		return idx;
//	}
//
//	// Get Index
//	int getClosestEntity(int cmpIndex)
//	{
//		float bestDistance = FLT_MAX;
//		int idx = -1;
//		auto cmpPos = entities[cmpIndex]->getTransform().Translation();
//		for (int i = 0; i < entities.size(); ++i)
//		{
//			if(cmpIndex == i) continue;
//			float dist = glm::distance(
//				cmpPos, entities[i]->getTransform().Translation());
//			if (dist < bestDistance)
//			{
//				bestDistance = dist;
//				idx = i;
//			}
//		}
//		return idx;
//	}
//
//// 	template<typename T> int getEntity(std::string name)
// 	{
// 		for (auto i = 0; i < entities.size(); ++i)
// 		{
// 			if (entities[i]->hasMod<T>() &&
// 				Stricmp(entities[i]->getMod<T>().Name().c_str(), name.c_str()) == 0)
// 			{
// 				return i;
// 			}
// 		}
// 		return -1;
// 	}
//};