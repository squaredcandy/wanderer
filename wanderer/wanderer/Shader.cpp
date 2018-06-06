#include "Shader.h"

namespace Wanderer::Engine::Shaders
{
	ShaderID currentShader;
	std::map<ShaderID, Shader> shaders;

	void CheckCompileErrors(GLuint shader, std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}

	Shader InitShader(std::array<std::string, 3> shaderPaths)
	{
		Shader newShader;
		// 1. retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		std::ifstream gShaderFile;
		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
		gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
		std::string v(shaderPaths[0]);
		auto start = v.find_last_of('/') + 1;
		auto len = v.find_last_of('.') - start;
		try
		{
			// open files
			vShaderFile.open(shaderPaths[0]);
			fShaderFile.open(shaderPaths[1]);
			std::stringstream vShaderStream, fShaderStream;
			// read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
			// if geometry shader path is present, also load a geometry shader
			if (!shaderPaths[2].empty())
			{
				gShaderFile.open(shaderPaths[2]);
				std::stringstream gShaderStream;
				gShaderStream << gShaderFile.rdbuf();
				gShaderFile.close();
				geometryCode = gShaderStream.str();
			}
		}
		catch (std::ifstream::failure e)
		{

			std::cout << "ERROR :: SHADER " << v.substr(start, len)
				<< " :: FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char * fShaderCode = fragmentCode.c_str();
		// 2. compile shaders
		unsigned int vertex, fragment;
		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, nullptr);
		glCompileShader(vertex);
		CheckCompileErrors(vertex, "VERTEX");
		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, nullptr);
		glCompileShader(fragment);
		CheckCompileErrors(fragment, "FRAGMENT");
		// if geometry shader is given, compile geometry shader
		unsigned int geometry;
		if (!shaderPaths[2].empty())
		{
			const char * gShaderCode = geometryCode.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, nullptr);
			glCompileShader(geometry);
			CheckCompileErrors(geometry, "GEOMETRY");
		}
		// shader Program
		newShader.id = glCreateProgram();
		glAttachShader(newShader.id, vertex);
		glAttachShader(newShader.id, fragment);
		if (!shaderPaths[2].empty())
			glAttachShader(newShader.id, geometry);
		glLinkProgram(newShader.id);
		CheckCompileErrors(newShader.id, "PROGRAM");
		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (!shaderPaths[2].empty())
			glDeleteShader(geometry);
		newShader.initialized = true;

		std::cout << "Shader Loaded: " << v.substr(start, len) << std::endl;
		return newShader;
	}

	void CreateShader(ShaderID key, std::string name, 
					  bool useVert, bool useFrag, bool useGeo)
	{
		auto path = "Data/Shaders/";
		std::array<std::string, 3> shaderPaths;
		if(useVert) shaderPaths[0] = path + name + ".vert";
		if(useFrag) shaderPaths[1] = path + name + ".frag";
		if(useGeo)  shaderPaths[2] = path + name + ".geo";
		shaders[key] = InitShader(shaderPaths);
	}

	void Cleanup()
	{
		for (auto& shader : shaders)
		{
			glDeleteBuffers(1, &shader.second.id);
		}
	}

	void CheckInitialization(ShaderID key)
	{
		if (shaders.empty())
		{
			std::cout << "No Available Shaders" << std::endl;
		}
		else if (!shaders[key].initialized)
		{
			std::cout << "Shader ID: " << shaders[key].id 
				<< " not initialized" << std::endl;
		}
	}

	void SetCurrentShader(ShaderID key)
	{
		CheckInitialization(key);
		glUseProgram(shaders[key].id);
		currentShader = key;
	}

	inline GLint GetUniformLocation(const std::string& name)
	{
		return glGetUniformLocation(shaders[currentShader].id, name.c_str());
	}

	void setBool(const std::string &name, bool value)
	{
		glUniform1i(GetUniformLocation(name), (int) value);
	}

	void setInt(const std::string & name, int value) 
	{
		glUniform1i(GetUniformLocation(name), value);
	}

	void setFloat(const std::string &name, float value) 
	{
		glUniform1f(GetUniformLocation(name), value);
	}

	void setVec2(const std::string &name, const glm::vec2 &value) 
	{
		glUniform2fv(GetUniformLocation(name), 1, &value[0]);
	}

	void setVec2(const std::string &name, float x, float y) 
	{
		glUniform2f(GetUniformLocation(name), x, y);
	}

	void setVec3(const std::string &name, const glm::vec3 &value) 
	{
		glUniform3fv(GetUniformLocation(name), 1, &value[0]);
	}

	void setVec3(const std::string &name, float x, float y, float z) 
	{
		glUniform3f(GetUniformLocation(name), x, y, z);
	}

	void setVec4(const std::string &name, const glm::vec4 &value) 
	{
		glUniform4fv(GetUniformLocation(name), 1, &value[0]);
	}

	void setVec4(const std::string &name, float x, float y, float z, float w)
	{
		glUniform4f(GetUniformLocation(name), x, y, z, w);
	}

	void setMat2(const std::string &name, const glm::mat2 &mat) 
	{
		glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
	}

	void setMat3(const std::string &name, const glm::mat3 &mat) 
	{
		glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
	}

	void setMat4(const std::string &name, const glm::mat4 &mat) 
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
	}
}