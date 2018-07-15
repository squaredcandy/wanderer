#pragma once

#include "Entity.h"
#include "Mods.h"

//#include <optional>
#include <vector>

namespace Wanderer::Engine::Entities
{
	extern EntityID playerID;

	extern std::vector<std::unique_ptr<Entity>> entities;

	void Update(float deltaTime);
	void RemoveInactive();
	void RemoveAll();
	std::size_t Size();
	Entity * AddEntity(std::string name = "",
					   glm::vec3 translate = glm::vec3(0, 5, 0),
					   glm::vec3 rotate = glm::vec3(),
					   glm::vec3 scale = glm::vec3(1, 1, 1));
	Entity * Get(EntityID id) noexcept;
	std::vector<Entity*> GetAll();

	template<typename T> std::vector<Entity*> GetAll()
	{
		std::vector<Entity*> found;
		for (auto & entity : entities)
		{
			auto ptr = entity.get();
			if (ptr->HasMod<T>())
			{
				found.emplace_back(ptr);
			}
		}
		return found;
	}

	template<typename T, typename S, typename... Arg> std::vector<Entity*> GetAll()
	{
		std::vector<Entity*> found;
		std::vector<Entity*> total;
		for (auto & entity : entities)
		{
			auto ptr = entity.get();
			if (ptr->HasMod<T>())
			{
				found.emplace_back(ptr);
			}
		}
		auto returned = GetAll<S, Arg...>();
		for (auto& f : found)
		{
			for (auto& r : returned)
			{
				if (r == f)
				{
					total.emplace_back(f);
				}
			}
		}
		return total;
	}
	
}