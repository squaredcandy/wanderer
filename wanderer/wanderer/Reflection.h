#pragma once

#include <any>
#include <string>
#include <map>
#include <typeindex>
#include <functional>

#include <ImGui/imgui.h>

#include "ModTransform.h"

#define REFLECTARGS std::string& name, std::string& typeName, std::any& var

using ReflectFunction = std::function<void(std::string&, std::string&, std::any&)>;

namespace Wanderer::Engine::Reflection
{
	extern ImVec4 varColor;
	extern std::unordered_map<std::type_index, ReflectFunction> reflectMap;
	extern std::unordered_map<std::type_index, ReflectFunction> reflectEditMap;

	void inline FormatName(std::string& name);
	void inline FormatType(std::string& type);

	void ReflectEdit(std::string name, std::any var, bool optimization);
	void Reflect(std::string name, std::any var, bool optimization);

	void AddReflect(bool editable, std::type_index index, ReflectFunction value);
	void AddDefaultReflects();
}

using namespace Wanderer::Engine;
// Faster Version of Edit Reflection but we need to supply a name
// A: Variable
// B: Name
#define EDITVALUEN(A,B)	Reflection::ReflectEdit(B, std::any(&A), false)

// Faster Version of View Reflection but we need to supply a name
// A: Variable
// B: Name
#define PRINTVALUEN(A,B) Reflection::Reflect(B, std::any(A), false)

// Slower Version of Edit Reflection without the name requirement
// A: Variable
#define EDITVALUE(A)	Reflection::ReflectEdit(std::string{#A}, std::any(&A), true)

// Slower Version of View Reflection without the name requirement
// A: Variable
#define PRINTVALUE(A)	Reflection::Reflect(std::string{#A}, std::any(A), true)