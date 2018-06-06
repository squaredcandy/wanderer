#pragma once

#include <ImGui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <bitset>
#include <array>

class Module;
class Entity;

using ModuleID = std::size_t;

inline ModuleID GetModTypeID()
{
	// Save the last ID we used
	static ModuleID lastID = 0;
	// Return that ID and increment
	return lastID++;
}

// Return the same ID whenever we try to get a module
template <typename T> inline ModuleID GetModTypeID() noexcept
{
	static ModuleID typeID = GetModTypeID();
	return typeID;
}

// Static Variable
// 32 for x86 compatibility
constexpr std::size_t maxModule = 32;

using ModuleBitSet = std::bitset<maxModule>;
using ModuleArray = std::array<Module*, maxModule>;
using ModuleVector = std::vector<Module*>;

// Module Class
class Module
{
public:
	Entity * entity;

	virtual ~Module() = default;

	// Used to initialize dependencies
	virtual void Init() {};

	// Used to update Module
	virtual void Update(float deltaTime) {};

	virtual void GUI() {};
};