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
	void CreateVAO(Mesh& mesh);
	void CreateVBO(Mesh& mesh, GLuint dataSize);
	void CreateVBO(GLuint& vao, GLuint& vbo, GLuint64 size, 
				   void * data, GLuint drawType);
	void CreateEBO(Mesh& mesh);
	void AddAttribute(Mesh& mesh, int attribute, int dataSize, 
					  GLuint dataLength, GLuint64 offset);
	void AddAttribute(GLuint& vao, GLuint& vbo, int attribute, int dataSize,
					  GLuint dataLength, GLuint64 offset);
	void AddInstancedAttribute(Mesh& mesh, int attribute, int dataSize, 
							   GLuint dataLength, GLuint64 offset);
	void AddInstancedAttribute(GLuint& vao, GLuint& vbo, int attribute, 
							   int dataSize, GLuint dataLength, GLuint64 offset);
	Mesh * GetModel(MeshID key);
	Mesh * LoadModel(MeshID key, std::string path);
	void Cleanup();
}