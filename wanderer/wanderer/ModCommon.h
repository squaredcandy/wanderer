#pragma once

#include "Module.h"

using EntityID = std::size_t;

class ModCommon : public Module
{
private:
	std::string name;
	EntityID targetID;
public:
	explicit ModCommon(std::string _name = "", EntityID _target = 0) : 
		name(_name), targetID(_target)
	{

	}

	virtual ~ModCommon() override = default;

	void Init() override
	{

	}

	void Update(float deltaTime) override
	{

	}

	void GUI() override
	{

	}
};