#pragma once

#include <map>
#include <string>
#include <iostream>
#include <vector>
#include <memory>

#include <GL/gl3w.h>
#include <glm/glm.hpp>

struct Texture
{
	GLuint textureID;
	int width;
	int height;
	int nrChannels;
};

struct AnimTexture : Texture
{
	bool animated;
	int animWidth, animheight, animLength;
};

struct Material
{
	enum TextureType
	{
		// Texture Maps
		MAP_DIFFUSE,
		MAP_NORMAL,
		MAP_SPECULAR,
		MAP_DISPLACEMENT,
		MAP_AO,

		// World Maps
		MAP_HEIGHT,
		
		// Anim Maps
		MAP_DIFFUSE_ANIM
	};
	std::map<TextureType, std::unique_ptr<Texture>> textures;
};

using TextureID = unsigned int;

struct Chunk;

namespace Wanderer::Engine::Textures
{
	void Cleanup();
	Material * CreateMaterial(TextureID id);
	Material * GetMaterial(TextureID id);
	void LoadStaticMaterial(TextureID id, std::string fileName, Material::TextureType texType);
	//void LoadMaterial(TextureID id, std::string filename);
	void LoadWorldMap(TextureID id, std::vector<float>& chunk, int size);
}