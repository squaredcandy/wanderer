#pragma once

#include <any>
#include <string>
#include <map>
#include <typeindex>
#include <functional>

#include <ImGui/imgui.h>

#include "ModTransform.h"

#define REFLECTARGS std::string name, std::string typeName, std::any var

using ReflectFunction = std::function<void(std::string, std::string, std::any)>;

namespace Wanderer::Engine::Reflection
{
	extern ImVec4 varColor;
	extern std::unordered_map<std::type_index, ReflectFunction> reflectMap;
	extern std::unordered_map<std::type_index, ReflectFunction> reflectEditMap;
	
	void inline FormatName(std::string& name);
	void inline FormatType(std::string& type);

	void ReflectEdit(std::string name, std::any var);
	void Reflect(std::string name, std::any var);

	void AddReflect(bool editable, std::type_index index, ReflectFunction value);
	void AddDefaultReflects();
}

#define EDITVALUE(A)	Wanderer::Engine::Reflection::ReflectEdit(std::string{#A}, std::any(&A))
#define PRINTVALUE(A)	Wanderer::Engine::Reflection::Reflect(std::string{#A}, std::any(A))