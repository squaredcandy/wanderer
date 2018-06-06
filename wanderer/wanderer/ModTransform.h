#pragma once

#include "Module.h"

struct Transform
{
	glm::vec3 translation;
	glm::vec3 rotation;
	glm::vec3 scale;
	Transform(glm::vec3 t = { 0, 0, 0 },
			  glm::vec3 r = { 0, 0, 0 },
			  glm::vec3 s = { 1, 1, 1 }) :
		translation(t), 
		rotation(r), 
		scale(s) {}
	~Transform() = default;
};

class ModTransform : public Module
{
private:
	Transform transform;
	Transform targetTransform;
public:
	ModTransform(glm::vec3 translation = { 0, 0, 0 },
				 glm::vec3 rotation = { 0, 0, 0 },
				 glm::vec3 scale = { 1, 1, 1 }) :
		transform(translation, rotation, scale),
		targetTransform(translation, rotation, scale) {}

	virtual ~ModTransform() override = default;

	void Update(float deltaTime) override
	{

	}

	void GUI() override
	{

	}

	inline glm::vec3 GetTranslate() { return transform.translation; }
	inline glm::vec3 GetRotate() { return transform.rotation; }
	inline glm::vec3 GetScale() { return transform.scale; }

};