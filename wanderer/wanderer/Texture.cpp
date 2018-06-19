#include "Texture.h"

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

	Material * GetMaterial(TextureID id)
	{
		if (materials.find(id) == materials.end()) return nullptr;
		return &materials[id];
	}

	void LoadMaterial(TextureID id, std::string filename)
	{

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