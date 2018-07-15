#pragma once

#include "Module.h"
#include "Random.h"

class ModLightMod : public Module
{
private:
	glm::vec3 start, destination;
	float progress, radius, speed;
public:
	glm::vec3 startPoint;

	ModLightMod()
	{

	}

	virtual ~ModLightMod() override = default;

	glm::vec3 RandomPointInCircle()
	{
		return glm::normalize(glm::vec3{
			Wanderer::Engine::Random::RandomFloat(-1, 1),
			Wanderer::Engine::Random::RandomFloat(-1, 1),
			Wanderer::Engine::Random::RandomFloat(-1, 1)
		});
	}

	void NewDestination()
	{
		destination = RandomPointInCircle() * radius + startPoint;
	}

	void Init() override
	{
		if (!entity->HasMod<ModLight>())
		{
			entity->AddMod<ModLight>();
		}
		startPoint = entity->GetTransform().GetTranslate();
		start = startPoint;
		radius = 1.f;
		speed = 2.f;
		progress = 0.f;
		NewDestination();
	}
	
	void Update(float deltaTime) override
	{
		bool reached = false;
		progress += speed * deltaTime;
		if (progress >= 1.f)
		{
			progress = 1.f;
			reached = true;
		}

		entity->GetTransform().GetTranslate() = glm::mix(start, destination, progress);

		if (reached)
		{
			start = destination;
			NewDestination();
			progress = 0.f;
		}
	}

	void GUI() override
	{
		if (ImGui::TreeNode("Light Module Modifier"))
		{
			ImGui::DragFloat3("Start Point", glm::value_ptr(startPoint));
			//ImGui::DragFloat3("Destination", glm::value_ptr(destination));
			ImGui::DragFloat("Radius", &radius);
			ImGui::DragFloat("Speed", &speed);
			ImGui::TreePop();
		}
	}
};