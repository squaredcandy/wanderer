#pragma once

#include <map>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <any>

#include <GL/gl3w.h>
#include <glm/glm.hpp>

struct Texture
{
	GLuint textureID;
	int width;
	int height;
	int nrChannels;

	std::map<std::string, std::any> vars;

	Texture() {};
	virtual ~Texture() = default;
};

struct Material
{
	enum TextureType
	{
		// Texture Maps
		MAP_DIFFUSE,
		MAP_NORMAL,
		MAP_METALLIC,
		MAP_ROUGHNESS,
		MAP_AO,

		// Cubemap
		MAP_CUBEMAP,
		MAP_ENVMAP,

		// World Maps
		MAP_HEIGHT,
		
		// Anim Maps
		MAP_DIFFUSE_ANIM
	};
	std::map<TextureType, Texture> textures;

	Material() {};
	virtual ~Material() = default;
};

static std::map<Material::TextureType, std::string> texToString{
	{Material::MAP_DIFFUSE,	     "Diffuse"},
	{Material::MAP_NORMAL,	     "Normal"},
	{Material::MAP_METALLIC,     "Metallic"},
	{Material::MAP_ROUGHNESS,    "Roughness"},
	{Material::MAP_AO,		     "AO"},
	{Material::MAP_CUBEMAP,	     "Cubemap"},
	{Material::MAP_ENVMAP,	     "Environment"},
	{Material::MAP_HEIGHT,	     "Height"},
	{Material::MAP_DIFFUSE_ANIM, "Diffuse Anim"}
};

struct Chunk;

namespace Wanderer::Engine::Textures
{
	void Cleanup();
	Material * CreateMaterial(std::string id);
	Material * GetMaterial(std::string id);
	std::map<std::string, Material>& GetAllMaterials();
	void CreateEmptyMaterial(std::string id, Material::TextureType texType, GLuint rgbType, int size);
	void LoadStaticMaterial(std::string id, std::string fileName, Material::TextureType texType);
	void LoadHDREnvMap(std::string id, std::string filename);
	void CreateEmptyCubemap(std::string id, int size,
							GLuint minFilter, bool generateMips = false);
	void LoadWorldMap(std::string id, std::vector<float>& chunk, int size);
}