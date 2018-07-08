#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Wanderer::Engine::Textures
{
	std::map<TextureID, Material> materials;

	void Cleanup()
	{
		for (auto& mat : materials)
		{
			for (auto& tex : mat.second.textures)
			{
				glDeleteTextures(1, &tex.second->textureID);
			}
		}
	}

	Material * CreateMaterial(TextureID id)
	{
		Material newMat;
		materials[id] = std::move(newMat);
		return &materials[id];
	}

	Material * GetMaterial(TextureID id)
	{
		if (materials.find(id) == materials.end()) return nullptr;
		return &materials[id];
	}

	void LoadStaticMaterial(TextureID id, std::string filename, 
							Material::TextureType texType)
	{
		auto newTex = std::make_unique<Texture>();

		glGenTextures(1, &newTex->textureID);
		glBindTexture(GL_TEXTURE_2D, newTex->textureID);
		//glGenerateMipmap(newTex->textureID);
		stbi_set_flip_vertically_on_load(true);
		std::string file{ "Data/Textures/" };
		file.append(filename);
		unsigned char * data = stbi_load(file.c_str(),
										 &newTex->width,
										 &newTex->height,
										 &newTex->nrChannels,
										 0);
		GLenum format;
		if (newTex->nrChannels == 1) { format = GL_RED; }
		else if (newTex->nrChannels == 3) { format = GL_RGB; }
		else if (newTex->nrChannels == 4) { format = GL_RGBA; }
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, format,
						 newTex->width,
						 newTex->height,
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

	void LoadWorldMap(TextureID id, std::vector<float>& chunk, int size)
	{
		auto newTex = std::make_unique<Texture>();
		Material newMat;

		glGenTextures(1, &newTex->textureID);
		glBindTexture(GL_TEXTURE_2D, newTex->textureID);
		newTex->width = size;
		newTex->height = size;
		newTex->nrChannels = 1;
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