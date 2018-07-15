#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Wanderer::Engine::Textures
{
	std::map<std::string, Material> materials;

	void Cleanup()
	{
		for (auto& mat : materials)
		{
			for (auto& tex : mat.second.textures)
			{
				glDeleteTextures(1, &tex.second.textureID);
			}
		}
	}

	Material * CreateMaterial(std::string id)
	{
		Material newMat;
		materials[id] = std::move(newMat);
		return &materials[id];
	}

	Material * GetMaterial(std::string id)
	{
		if (materials.find(id) == materials.end()) return nullptr;
		return &materials[id];
	}

	std::map<std::string, Material>& GetAllMaterials()
	{
		return materials;
	}

	void LoadStaticMaterial(std::string id, std::string filename, 
							Material::TextureType texType)
	{
		auto newTex = Texture();

		//glGenerateMipmap(newTex.textureID);
		//stbi_set_flip_vertically_on_load(true);
		std::string file{ "Data/Textures/" };
		file.append(filename);
		auto * data = stbi_load(file.c_str(),
								&newTex.width,
								&newTex.height,
								&newTex.nrChannels,
								0);
		GLenum format;
		if (newTex.nrChannels == 1) { format = GL_RED; }
		else if (newTex.nrChannels == 3) { format = GL_RGB; }
		else if (newTex.nrChannels == 4) { format = GL_RGBA; }
		if (data)
		{
			glGenTextures(1, &newTex.textureID);
			glBindTexture(GL_TEXTURE_2D, newTex.textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format,
						 newTex.width, newTex.height,
						 0, format, GL_UNSIGNED_BYTE, data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
							format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
							format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			const std::string& f(filename);
			std::cout << "Texture Loaded: " << f.substr(f.find_last_of('/') + 1) << std::endl;
		}
		else
		{
			std::cout << "ERROR :: TEXTURE " << filename << " :: Texture not loaded" << std::endl;
		}
		stbi_image_free(data);
		materials[id].textures[texType] = std::move(newTex);
	}

	void CreateEmptyMaterial(std::string id, Material::TextureType texType, GLuint rgbType, int size)
	{
		auto newTex = Texture();
		glGenTextures(1, &newTex.textureID);

		// pre-allocate enough memory for the LUT texture.
		glBindTexture(GL_TEXTURE_2D, newTex.textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 
					 size, size, 0, rgbType, GL_FLOAT, nullptr);
		// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		materials[id].textures[texType] = std::move(newTex);
	}

	void LoadHDREnvMap(std::string id, std::string filename)
	{
		auto newTex = Texture();

		stbi_set_flip_vertically_on_load(true);
		std::string file{ "Data/Textures/" };
		file.append(filename);
		auto * data = stbi_loadf(file.c_str(),
								 &newTex.width,
								 &newTex.height,
								 &newTex.nrChannels,
								 0);
		if (data)
		{
			glGenTextures(1, &newTex.textureID);
			glBindTexture(GL_TEXTURE_2D, newTex.textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 
						 newTex.width, newTex.height, 
						 0, GL_RGB, GL_FLOAT, data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			const std::string& f(filename);
			std::cout << "Texture Loaded: " << f.substr(f.find_last_of('/') + 1) << std::endl;
		}
		else
		{
			std::cout << "ERROR :: TEXTURE " << filename << " :: Texture not loaded" << std::endl;
		}
		stbi_image_free(data);
		materials[id].textures[Material::MAP_ENVMAP] = std::move(newTex);
	}

	void CreateEmptyCubemap(std::string id, int size, GLuint minFilter, bool generateMips)
	{
		auto newTex = Texture();
	
		glGenTextures(1, &newTex.textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, newTex.textureID);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 
						 GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		// enable pre-filter mipmap sampling (combatting visible dots artifact)
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if (generateMips) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		materials[id].textures[Material::MAP_CUBEMAP] = std::move(newTex);
	}

	void LoadWorldMap(std::string id, std::vector<float>& chunk, int size)
	{
		auto newTex = Texture();
		Material newMat;

		glGenTextures(1, &newTex.textureID);
		glBindTexture(GL_TEXTURE_2D, newTex.textureID);
		newTex.width = size;
		newTex.height = size;
		newTex.nrChannels = 1;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, size, size, 0, GL_RED, GL_FLOAT, &chunk.front());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		newMat.textures[Material::MAP_HEIGHT] = std::move(newTex);
		materials[id] = std::move(newMat);
	}
}