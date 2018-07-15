#include "Mesh.h"

namespace Wanderer::Engine::Meshes
{
	std::map<MeshID, Mesh> meshes;

	void InitBuffer(Mesh& mesh)
	{
		// Create and bind vertex and index data

		GLuint size = sizeof(Vertex);
		CreateVAO(mesh);
		CreateVBO(mesh, size);
		CreateEBO(mesh);

		AddAttribute(mesh, 0, 3, size, offsetof(Vertex, Vertex::Position));
		AddAttribute(mesh, 1, 3, size, offsetof(Vertex, Vertex::Normal));
		AddAttribute(mesh, 2, 2, size, offsetof(Vertex, Vertex::TexCoords));
		AddAttribute(mesh, 3, 3, size, offsetof(Vertex, Vertex::Tangent));
		AddAttribute(mesh, 4, 3, size, offsetof(Vertex, Vertex::Bitangent));
	}

	void CreateVAO(Mesh& mesh)
	{
		glGenVertexArrays(1, &mesh.VAO);
		glBindVertexArray(mesh.VAO);
		glBindVertexArray(0);
	}

	void CreateVBO(Mesh& mesh, GLuint dataSize)
	{
		glGenBuffers(1, &mesh.VBO);
		glBindVertexArray(mesh.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
		glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex),
					 &mesh.vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void CreateVBO(GLuint& vao, GLuint& vbo, GLuint64 size, void * data, GLuint drawType)
	{
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, size, data, drawType);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void CreateEBO(Mesh& mesh)
	{
		glGenBuffers(1, &mesh.EBO);
		glBindVertexArray(mesh.VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(GLuint),
					 &mesh.indices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void AddAttribute(Mesh& mesh, int attribute, int dataSize, 
					  GLuint dataLength, GLuint64 offset)
	{
		glBindVertexArray(mesh.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
 		glEnableVertexAttribArray(attribute);
		glVertexAttribPointer(attribute, dataSize, GL_FLOAT, GL_FALSE, 
							  dataLength, (GLvoid*) offset);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void AddAttribute(GLuint& vao, GLuint& vbo, int attribute, int dataSize, 
					  GLuint dataLength, GLuint64 offset)
	{
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(attribute);
		glVertexAttribPointer(attribute, dataSize, GL_FLOAT, GL_FALSE,
							  dataLength, (GLvoid*) offset);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void AddInstancedAttribute(Mesh& mesh, int attribute, int dataSize, 
							   GLuint dataLength, GLuint64 offset)
	{
		glBindVertexArray(mesh.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
 		glEnableVertexAttribArray(attribute);
		glVertexAttribPointer(attribute, dataSize, GL_FLOAT, GL_FALSE, 
							  dataLength, (GLvoid*) offset);
		glVertexAttribDivisor(attribute, 1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void AddInstancedAttribute(GLuint& vao, GLuint& vbo, int attribute, int dataSize,
					  GLuint dataLength, GLuint64 offset)
	{
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(attribute);
		glVertexAttribPointer(attribute, dataSize, GL_FLOAT, GL_FALSE,
							  dataLength, (GLvoid*) offset);
		glVertexAttribDivisor(attribute, 1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	Mesh ProcessMesh(aiMesh * mesh)
	{
		Mesh newMesh = Mesh();
		auto& vertices = newMesh.vertices;
		auto& indices = newMesh.indices;
		// Walk through each of the mesh's vertices
		vertices.reserve(mesh->mNumVertices);
		for (GLuint i = 0; i < mesh->mNumVertices; ++i)
		{
			vertices.emplace_back(Vertex());
			auto& vertex = vertices.back();

			// Position
			vertex.Position.x = mesh->mVertices[i].x;
			vertex.Position.y = mesh->mVertices[i].y;
			vertex.Position.z = mesh->mVertices[i].z;

			// normals
			vertex.Normal.x = mesh->mNormals[i].x;
			vertex.Normal.y = mesh->mNormals[i].y;
			vertex.Normal.z = mesh->mNormals[i].z;

			// texture coordinates
			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
				vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
			}
			else
			{
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			}

			// tangent
			vertex.Tangent.x = mesh->mTangents[i].x;
			vertex.Tangent.y = mesh->mTangents[i].y;
			vertex.Tangent.z = mesh->mTangents[i].z;

			// bitangent
			vertex.Bitangent.x = mesh->mBitangents[i].x;
			vertex.Bitangent.y = mesh->mBitangents[i].y;
			vertex.Bitangent.z = mesh->mBitangents[i].z;
		}
		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (GLuint i = 0; i < mesh->mNumFaces; ++i)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (GLuint j = 0; j < face.mNumIndices; ++j)
				indices.emplace_back(face.mIndices[j]);
		}

		InitBuffer(newMesh);

		return newMesh;
	}
	
	void ProcessNode(MeshID key, aiNode * node, const aiScene * scene)
	{
		for (GLuint i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
			//meshes.emplace_back(processMesh(mesh, scene));
			// This wont work for multimaps
			
			//meshes.insert(std::make_pair(key, ProcessMesh(mesh)));
			if (meshes.find(key) != meshes.end())
			{
				printf("Overriding mesh at key %d", key);
			}
			meshes[key] = ProcessMesh(mesh);
		}

		for (GLuint i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNode(key, node->mChildren[i], scene);
		}
	}
	
	Mesh * LoadModel(MeshID key, std::string path)
	{
		Assimp::Importer importer;
		path.insert(0, "Data/Meshes/");
		const aiScene * scene = 
			importer.ReadFile(path, 
							  //aiProcessPreset_TargetRealtime_MaxQuality);
							  aiProcess_Triangulate |
							  //aiProcess_FlipUVs |
							  aiProcess_CalcTangentSpace |
							  aiProcess_OptimizeMeshes | 
							  aiProcess_OptimizeGraph | 
							  aiProcess_JoinIdenticalVertices);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || 
			!scene->mRootNode)
		{
			printf("ERROR :: ASSIMP :: %s\n", importer.GetErrorString());
			return nullptr;
		}
		ProcessNode(key, scene->mRootNode, scene);
		printf("Model Loaded: %s\n", 
			   path.substr(path.find_last_of('/') + 1).c_str());
		return &meshes[key];
	}

	void Cleanup()
	{
		for (auto& mesh : meshes)
		{
			glDeleteVertexArrays(1, &mesh.second.VAO);
			glDeleteBuffers(1, &mesh.second.VBO);
			glDeleteBuffers(1, &mesh.second.EBO);
		}
	}

	Mesh * GetModel(MeshID key)
	{
		if (meshes.find(key) == meshes.end()) return nullptr;
		return &meshes[key];
	}
}
