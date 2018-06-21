#pragma once

#include "Mesh.h"
#include "Engine.h"
#include "Flags.h"
#include "Debug.h"
#include "Font.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "World.h"

const MeshID	MESH_SPHERE			= 00;
const MeshID	MESH_PLANE			= 01;

const ShaderID	SHADER_TERRAIN		= 00;
const ShaderID	SHADER_TESS			= 01;
const ShaderID	SHADER_NORM			= 02;

const CameraID	CAMERA_PLAYER		= 00;

const TextureID TEX_TERRAIN_HEIGHT	= 00;

const int		CHUNK_LENGTH		= 1;

struct Buffer
{
	GLuint frame;
	GLuint color;
	GLuint depth;

	Buffer() = default;
	~Buffer() = default;
};

using BufferID = unsigned int;

namespace Wanderer::Engine::Render
{
	extern GLuint sampleSize;

	extern std::map<BufferID, Buffer> buffers;

	// Set the buffers we will use 
	extern BufferID drawBufferID;
	extern BufferID msaaBufferID;

	// Vector to store the opaque objects
	extern std::vector<ModelInstance> opaqueObjects;

	// Map to store the transparent objects
	extern std::multimap<float, ModelInstance> transparentObjects;

	void Cleanup();
	void CheckFrameBufferStatus(std::string bufferName);

	void AddBuffer(BufferID bufferKey);

	void InitFrameBuffer(BufferID bufferKey);
	void InitColorBuffer(BufferID bufferKey, GLuint textureType);
	void InitDepthBuffer(BufferID bufferKey);
	
	void BindTextureToFramebuffer(Buffer buffer, 
								  ImVec2& size, 
								  GLuint textureType);
	void BindDepthToFrameBuffer(Buffer buffer, ImVec2& size, bool multisampled);

	void BeginFrameBufferDrawing(GLuint frameBuffer, ImVec2& size);
	void CopyMSAABufferToDrawBuffer(GLuint msaaBuffer, 
									GLuint drawBuffer, 
									ImVec2& size);
	void EndFrameBufferDrawing(GLuint drawBuffer, ImVec2& size, ImVec2& pos);
	void RenderWorld();
}