#include "Entities.h"

namespace Wanderer::Engine::Entities
{
	EntityID playerID;
	// We don't declare it in h file because we want to protect it from
	// directly accessed
	std::vector<std::unique_ptr<Entity>> entities;

	void Update(float deltaTime)
	{
		// We do this because we can potentually add an entity during the update
		for (auto i = 0; i < entities.size(); ++i)
		{
			entities[i]->Update(deltaTime);
		}
		RemoveInactive();
	}

	void RemoveInactive()
	{
		// Maybe make a new entity that gets their inv and drops it
		entities.erase(std::remove_if(std::begin(entities), std::end(entities),
									  [] (const std::unique_ptr<Entity> &e)
		{
			return !e->isActive();
		}), entities.end());
	}

	void RemoveAll()
	{
		entities.clear();
	}

	std::size_t Size()
	{
		return entities.size();
	}

	Entity * AddEntity(std::string name, glm::vec3 translate,
					   glm::vec3 rotate, glm::vec3 scale)
	{
		std::unique_ptr<Entity> uPtr = std::make_unique<Entity>();
		uPtr->AddMod<ModCommon>(name, uPtr->GetID());
		uPtr->AddMod<ModTransform>(translate, rotate, scale);
		entities.emplace_back(std::move(uPtr));

		return &*entities.back();
	}

	Entity * Get(EntityID id) noexcept
	{
		auto found = std::find_if(std::begin(entities), std::end(entities),
								  [=] (const std::unique_ptr<Entity>& e)
		{
			return (e->GetID() == id);
		});
		if (found != std::end(entities))
		{
			return (*found).get();
		}
		return nullptr;
	}

	std::vector<Entity*> GetAll()
	{
		std::vector<Entity*> found;
		for (auto & entity : entities)
		{
			auto ptr = entity.get();
			found.emplace_back(ptr);
		}
		return found;
	}
}