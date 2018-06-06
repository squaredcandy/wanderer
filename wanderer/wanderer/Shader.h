#pragma once



#include <map>
#include <array>
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
					  bool useGeo = false);
	void Cleanup();

	void SetCurrentShader(ShaderID key);

	void setBool(const std::string &name, bool value);
	void setInt(const std::string &name, int value);
	void setFloat(const std::string &name, float value);
	void setVec2(const std::string &name, const glm::vec2 &value);
	void setVec2(const std::string &name, float x, float y);
	void setVec3(const std::string &name, const glm::vec3 &value);
	void setVec3(const std::string &name, float x, float y, float z);
	void setVec4(const std::string &name, const glm::vec4 &value);
	void setVec4(const std::string &name, float x, float y, float z, float w);
	void setMat2(const std::string &name, const glm::mat2 &mat);
	void setMat3(const std::string &name, const glm::mat3 &mat);
	void setMat4(const std::string &name, const glm::mat4 &mat);
}