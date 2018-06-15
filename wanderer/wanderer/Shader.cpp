#include "Shader.h"

const std::size_t SHADER_LENGTH = 6;

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

	void CompileShader(std::string& filename, GLuint shaderType, 
					   std::string shaderTypeName, unsigned int& shader)
	{
		std::string code;
		std::ifstream fileStream;
		fileStream.exceptions (std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			fileStream.open(filename);
			std::stringstream ss;
			ss << fileStream.rdbuf();
			fileStream.close();
			code = ss.str().c_str();
		}
		catch (std::ifstream::failure e)
		{
			const std::string& v(filename);
			auto start = v.find_last_of('/') + 1;
			auto len = v.find_last_of('.') - start;
			std::cout << "ERROR :: SHADER " << v.substr(start, len)
				<< " :: FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		
		const char * cCode = code.c_str();
		shader = glCreateShader(shaderType);
		glShaderSource(shader, 1, &cCode, nullptr);
		glCompileShader(shader);
		CheckCompileErrors(shader, shaderTypeName);
	}

	Shader InitShader(std::array<std::string, SHADER_LENGTH> shaderPaths)
	{
		Shader newShader;

		unsigned int shaderCode[SHADER_LENGTH];
		
		const GLuint shaderType[SHADER_LENGTH] {
			GL_VERTEX_SHADER, 
			GL_FRAGMENT_SHADER, 
			GL_GEOMETRY_SHADER, 
			GL_TESS_CONTROL_SHADER, 
			GL_TESS_EVALUATION_SHADER,
			GL_COMPUTE_SHADER
		};
		
		const std::string shaderName[SHADER_LENGTH] {
			"VERTEX",
			"FRAGMENT",
			"GEOMETRY",
			"TESS CONTROL",
			"TESS EVALUATION",
			"COMPUTE"
		};

		for (int i = 0; i < SHADER_LENGTH; ++i)
		{
			if (!shaderPaths[i].empty())
				CompileShader(shaderPaths[i], shaderType[i], 
							  shaderName[i], shaderCode[i]);
		}

		newShader.id = glCreateProgram();
		for (int i = 0; i < SHADER_LENGTH; ++i)
		{
			if (!shaderPaths[i].empty()) glAttachShader(newShader.id, shaderCode[i]);
		}
		glLinkProgram(newShader.id);
		CheckCompileErrors(newShader.id, "PROGRAM");

		for (int i = 0; i < SHADER_LENGTH; ++i)
		{
			if (!shaderPaths[i].empty()) glDeleteShader(shaderCode[i]);
		}

		newShader.initialized = true;

		const std::string& v(shaderPaths[0]);
		auto start = v.find_last_of('/') + 1;
		auto len = v.find_last_of('.') - start;

		std::cout << "Shader Loaded: " << v.substr(start, len) << std::endl;
		return newShader;
	}

	void CreateShader(ShaderID key, std::string name, 
					  bool useVert, bool useFrag, 
					  bool useGeom, bool useTess, bool useComp)
	{
		auto path = "Data/Shaders/";
		std::array<std::string, SHADER_LENGTH> shaderPaths;
		if(useVert) shaderPaths[0] = path + name + ".vert";
		if(useFrag) shaderPaths[1] = path + name + ".frag";
		if(useGeom) shaderPaths[2] = path + name + ".geom";
		if(useTess)	shaderPaths[3] = path + name + ".tesc";
		if(useTess)	shaderPaths[4] = path + name + ".tese";
		if(useComp)	shaderPaths[5] = path + name + ".comp";
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

	void SetBool(const std::string &name, bool value)
	{
		glUniform1i(GetUniformLocation(name), (int) value);
	}

	void SetInt(const std::string & name, int value) 
	{
		glUniform1i(GetUniformLocation(name), value);
	}

	void SetFloat(const std::string &name, float value) 
	{
		glUniform1f(GetUniformLocation(name), value);
	}

	void SetVec2(const std::string &name, const glm::vec2 &value) 
	{
		glUniform2fv(GetUniformLocation(name), 1, &value[0]);
	}

	void SetVec2(const std::string &name, float x, float y) 
	{
		glUniform2f(GetUniformLocation(name), x, y);
	}

	void SetVec3(const std::string &name, const glm::vec3 &value) 
	{
		glUniform3fv(GetUniformLocation(name), 1, &value[0]);
	}

	void SetVec3(const std::string &name, float x, float y, float z) 
	{
		glUniform3f(GetUniformLocation(name), x, y, z);
	}

	void SetVec4(const std::string &name, const glm::vec4 &value) 
	{
		glUniform4fv(GetUniformLocation(name), 1, &value[0]);
	}

	void SetVec4(const std::string &name, float x, float y, float z, float w)
	{
		glUniform4f(GetUniformLocation(name), x, y, z, w);
	}

	void SetMat2(const std::string &name, const glm::mat2 &mat) 
	{
		glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
	}

	void SetMat3(const std::string &name, const glm::mat3 &mat) 
	{
		glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
	}

	void SetMat4(const std::string &name, const glm::mat4 &mat) 
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
	}
}