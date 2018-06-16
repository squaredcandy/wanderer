#include "Reflection.h"

namespace Wanderer::Engine::Reflection
{
	using namespace std::literals::string_literals;

	const std::string sVec2{ "glm::vec2" };
	const std::string sVec3{ "glm::vec3" };
	const std::string siVec2{ "glm::ivec2" };
	const std::string siVec3{ "glm::ivec3" };
	const std::string sMat4{ "glm::mat4" };
	const std::string sString{ "std::string" };
	const std::string sTransform{ "Transform" };

	ImVec4 varColor = { 0.5f, 0, 1.f, 1 };
	
	std::unordered_map<std::type_index, ReflectFunction> reflectMap;
	std::unordered_map<std::type_index, ReflectFunction> reflectEditMap;

	void FormatName(std::string& name)
	{
		name.reserve(50);
		std::size_t end;
		// Remove the c_str
		if ((end = name.find(".c_str()")) != std::string::npos) 
			name.erase(end, 9);
		// Remove the accessor
		if ((end = name.find(".")) != std::string::npos) 
			name = name.substr(end + 1);
	}

	void FormatType(std::string& type)
	{
		if (std::size_t end = type.find("struct "); end != std::string::npos)
		{
			type = type.substr(end + 7);
		}
	}

	void HandleTypeName(const std::string& typeName)
	{
		ImGui::TextColored(varColor, "%s", typeName.c_str());
		ImGui::SameLine();
	}

	void HandleIntEdit(REFLECTARGS)
	{
		ImGui::DragInt(name.c_str(), std::any_cast<int*>(var));
	}
	void HandleUIntEdit(REFLECTARGS)
	{
		ImGui::DragScalar(name.c_str(), ImGuiDataType_U32,
						  std::any_cast<unsigned int*>(var), 1.f,
						  nullptr, nullptr, "%u");
	}
	void HandleFloatEdit(REFLECTARGS)
	{
		ImGui::DragFloat(name.c_str(), std::any_cast<float*>(var));
	}
	void HandleDoubleEdit(REFLECTARGS)
	{
		ImGui::DragScalar(name.c_str(), ImGuiDataType_Double,
						  std::any_cast<double*>(var), 1.f, 
						  nullptr, nullptr, "%.3f");
	}
	void HandleStringEdit(REFLECTARGS)
	{
		ImGui::InputText(name.c_str(),
						 std::any_cast<std::string*>(var)->data(), 
						 50);
	}
	void HandleBoolEdit(REFLECTARGS)
	{
		ImGui::Checkbox(name.c_str(), std::any_cast<bool*>(var));
	}
	void HandleTransformEdit(REFLECTARGS)
	{
		auto a = std::any_cast<Transform*>(var);
		ImGui::Text(name.c_str());
		ImGui::DragFloat3("Transform", glm::value_ptr(a->translation));
		ImGui::DragFloat3("Rotate", glm::value_ptr(a->rotation));
		ImGui::DragFloat3("Scale", glm::value_ptr(a->scale));
	}
	void HandleVec2Edit(REFLECTARGS)
	{
		ImGui::DragFloat2(name.c_str(),
						  glm::value_ptr(
							  *std::any_cast<glm::vec2*>(var)));
	}
	void HandleVec3Edit(REFLECTARGS)
	{
		ImGui::DragFloat3(name.c_str(),
						  glm::value_ptr(
							  *std::any_cast<glm::vec3*>(var)));
	}
	void HandleiVec2Edit(REFLECTARGS)
	{
		ImGui::DragInt2(name.c_str(),
						glm::value_ptr(
							*std::any_cast<glm::ivec2*>(var)));
	}
	void HandleiVec3Edit(REFLECTARGS)
	{
		ImGui::DragInt3(name.c_str(),
						  glm::value_ptr(
							  *std::any_cast<glm::ivec3*>(var)));
	}
	void HandleMat4Edit(REFLECTARGS)
	{
		auto a = std::any_cast<glm::mat4*>(var);
		ImGui::Text("%s", name.c_str());
		ImGui::PushID("##1");
		ImGui::DragFloat4("", glm::value_ptr(*a) + 0);
		ImGui::PopID();
		ImGui::PushID("##2");
		ImGui::DragFloat4("", glm::value_ptr(*a) + 4);
		ImGui::PopID();
		ImGui::PushID("##3");
		ImGui::DragFloat4("", glm::value_ptr(*a) + 8);
		ImGui::PopID();
		ImGui::PushID("##4");
		ImGui::DragFloat4("", glm::value_ptr(*a) + 12);
		ImGui::PopID();
	}

	void HandleVoid(REFLECTARGS)
	{
		HandleTypeName(typeName);
		ImGui::Text(name.c_str());
		ImGui::SameLine();
		ImGui::TextColored({ 1, 0, 0, 1 }, "NOT IMPLEMENTED");
	}
	void HandleInt(REFLECTARGS)
	{
		HandleTypeName(typeName);
		ImGui::Text("%s: %d", name.c_str(), std::any_cast<int>(var));
	}
	void HandleUInt(REFLECTARGS)
	{
		HandleTypeName(typeName);
		ImGui::Text("%s: %u", name.c_str(), 
					std::any_cast<unsigned int>(var));
	}
	void HandleFloat(REFLECTARGS)
	{
		HandleTypeName(typeName);
		ImGui::Text("%s: %.3f", name.c_str(), 
					std::any_cast<float>(var));
	}
	void HandleDouble(REFLECTARGS)
	{
		HandleTypeName(typeName);
		ImGui::Text("%s: %.3f", name.c_str(),
					std::any_cast<double>(var));
	}
	void HandleString(REFLECTARGS)
	{
		HandleTypeName(sString);
		ImGui::Text("std::string %s: %s", name.c_str(),
					std::any_cast<std::string>(var).c_str());
	}
	void HandleBool(REFLECTARGS)
	{
		HandleTypeName(typeName);
		ImGui::Text("%s: %s", name.c_str(),
					std::any_cast<bool>(var) 
					? "True" : "False");
	}
	void HandleTransform(REFLECTARGS)
	{
		HandleTypeName(sTransform);
		auto a = std::any_cast<Transform>(var);
		ImGui::Text("%s:\n", name.c_str()); 
		ImGui::Text("Translate: x: %7.3f y: %7.3f z: %7.3f\n"
					"Rotate:    x: %7.3f y: %7.3f z: %7.3f\n"
					"Scale:     x: %7.3f y: %7.3f z: %7.3f",
					a.translation.x, a.translation.y, a.translation.z,
					a.rotation.x, a.rotation.y, a.rotation.z,
					a.scale.x, a.scale.y, a.scale.z);
	}
	void HandleVec2(REFLECTARGS)
	{
		HandleTypeName(sVec2);
		auto a = std::any_cast<glm::vec2>(var);
		ImGui::Text("%s: x: %.3f y: %.3f",
					name.c_str(), a.x, a.y);
	}
	void HandleVec3(REFLECTARGS)
	{
		HandleTypeName(sVec3);
		auto a = std::any_cast<glm::vec3>(var);
		ImGui::Text("%s: x: %.3f y: %.3f z: %.3f",
					name.c_str(), a.x, a.y, a.z);
	}
	void HandleiVec2(REFLECTARGS)
	{
		HandleTypeName(siVec2);
		auto a = std::any_cast<glm::ivec2>(var);
		ImGui::Text("%s: x: %d y: %d",
					name.c_str(), a.x, a.y);
	}
	void HandleiVec3(REFLECTARGS)
	{
		HandleTypeName(siVec3);
		auto a = std::any_cast<glm::ivec3>(var);
		ImGui::Text("%s: x: %d y: %d z: %d",
					name.c_str(), a.x, a.y, a.z);
	}
	void HandleMat4(REFLECTARGS)
	{
		HandleTypeName(sMat4);
		auto a = std::any_cast<glm::mat4>(var);
		ImGui::Text("%s:", name.c_str());
		ImGui::Text(
			"%8.3f %8.3f %8.3f %8.3f\n"
			"%8.3f %8.3f %8.3f %8.3f\n"
			"%8.3f %8.3f %8.3f %8.3f\n"
			"%8.3f %8.3f %8.3f %8.3f",
			a[0][0], a[0][1], a[0][2], a[0][3],
			a[1][0], a[1][1], a[1][2], a[1][3],
			a[2][0], a[2][1], a[2][2], a[2][3],
			a[3][0], a[3][1], a[3][2], a[3][3]
		);
	}

	void ReflectEdit(std::string name, std::any var)
	{
		std::string type{ var.type().name() };
		// Remove the x64 from the type
#if _WIN64 || __x86_64__ || __ppc64__
		type = type.substr(0, type.size() - 9);
#endif
		FormatName(name);
		//FormatType(type);
		reflectEditMap[var.type()](name, type, var);
	}

	void Reflect(std::string name, std::any var)
	{
		std::string type{ var.type().name() };
		FormatName(name);
		//FormatType(type);
		reflectMap[var.type()](name, type, var);
	}

	void AddReflect(bool editable, std::type_index index, 
					ReflectFunction value)
	{
		if (editable) reflectEditMap.insert_or_assign(index, value);
		else reflectMap.insert_or_assign(index, value);
	}

	void AddDefaultReflects()
	{
		using namespace std;
		reflectMap[type_index(typeid(void))] = &HandleVoid;
		reflectMap[type_index(typeid(int))] = &HandleInt;
		reflectMap[type_index(typeid(unsigned int))] = &HandleUInt;
		reflectMap[type_index(typeid(float))] = &HandleFloat;
		reflectMap[type_index(typeid(double))] = &HandleDouble;
		reflectMap[type_index(typeid(string))] = &HandleString;
		reflectMap[type_index(typeid(const char *))] = &HandleVoid;
		reflectMap[type_index(typeid(bool))] = &HandleBool;
		reflectMap[type_index(typeid(Transform))] = &HandleTransform;
		reflectMap[type_index(typeid(glm::vec2))] = &HandleVec2;
		reflectMap[type_index(typeid(glm::vec3))] = &HandleVec3;
		reflectMap[type_index(typeid(glm::ivec2))] = &HandleiVec2;
		reflectMap[type_index(typeid(glm::ivec3))] = &HandleiVec3;
		reflectMap[type_index(typeid(glm::mat4))] = &HandleMat4;

		reflectEditMap[type_index(typeid(void*))] = &HandleVoid;
		reflectEditMap[type_index(typeid(int*))] = &HandleIntEdit;
		reflectEditMap[type_index(typeid(unsigned int*))] = &HandleUIntEdit;
		reflectEditMap[type_index(typeid(float*))] = &HandleFloatEdit;
		reflectEditMap[type_index(typeid(double*))] = &HandleDoubleEdit;
		reflectEditMap[type_index(typeid(string*))] = &HandleStringEdit;
		reflectEditMap[type_index(typeid(const char **))] = &HandleVoid;
		reflectEditMap[type_index(typeid(bool*))] = &HandleBoolEdit;
		reflectEditMap[type_index(typeid(Transform*))] = &HandleTransformEdit;
		reflectEditMap[type_index(typeid(glm::vec2*))] = &HandleVec2Edit;
		reflectEditMap[type_index(typeid(glm::vec3*))] = &HandleVec3Edit;
		reflectEditMap[type_index(typeid(glm::ivec2*))] = &HandleiVec2Edit;
		reflectEditMap[type_index(typeid(glm::ivec3*))] = &HandleiVec3Edit;
		reflectEditMap[type_index(typeid(glm::mat4*))] = &HandleMat4Edit;
	}
}