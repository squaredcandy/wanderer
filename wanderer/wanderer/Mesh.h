#pragma once

#include <vector>
#include <map>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL/gl3w.h>
#include <glm/gtc/type_ptr.hpp>

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

struct Mesh
{
	GLuint VAO, VBO, EBO;

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
};

using MeshID = unsigned int;

namespace Wanderer::Engine::Meshes
{
	void LoadModel(MeshID key, std::string path);
	void Cleanup();
}