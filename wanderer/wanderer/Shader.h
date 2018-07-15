#pragma once

#include <map>
#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/gl3w.h>
#include <glm/glm.hpp>

using ShaderID = unsigned int;

struct Shader
{
	bool initialized = false;
	GLuint id;
};

namespace Wanderer::Engine::Shaders
{
	void CreateShader(ShaderID key, 
					  std::string name, 
					  bool useVert = true, 
					  bool useFrag = true, 
					  bool useGeom = false,
					  bool useTess = false,
					  bool useComp = false);
	void Cleanup();

	void SetCurrentShader(ShaderID key);
	std::vector<GLchar> GetProgramError(ShaderID key);

	void SetBool(const std::string &name, bool value);
	void SetInt(const std::string &name, int value);
	void SetFloat(const std::string &name, float value);
	void SetFloat(const std::string &name, float * value, int len);
	void SetVec2(const std::string &name, const glm::vec2 &value);
	void SetVec2(const std::string &name, float x, float y);
	void SetVec3(const std::string &name, const glm::vec3 &value);
	void SetVec3(const std::string &name, float x, float y, float z);
	void SetVec4(const std::string &name, const glm::vec4 &value);
	void SetVec4(const std::string &name, float x, float y, float z, float w);
	void SetMat2(const std::string &name, const glm::mat2 &mat);
	void SetMat3(const std::string &name, const glm::mat3 &mat);
	void SetMat4(const std::string &name, const glm::mat4 &mat);
}