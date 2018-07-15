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
#include "Dungeon.h"

const MeshID	MESH_SPHERE			=  0U;
const MeshID	MESH_FLOOR			=  1U;
const MeshID	MESH_WALL			=  2U;
const MeshID	MESH_DOORARCH		=  4U;
const MeshID	MESH_GATE			=  5U;
const MeshID	MESH_DOORCAP		=  6U;
const MeshID	MESH_UPSTAIR		=  7U;
const MeshID	MESH_DOWNSTAIR		=  8U;
const MeshID	MESH_SPRITE			=  9U;
									   
// PBR Shaders
const ShaderID	SHADER_PBR			=  1U;
const ShaderID	SHADER_EQUTOCUBE	=  2U;
const ShaderID	SHADER_IRRADIANCE	=  3U;
const ShaderID	SHADER_PREFILTER	=  4U;
const ShaderID	SHADER_BRDF			=  5U;
const ShaderID	SHADER_BACKGROUND	=  6U;

const ShaderID	SHADER_TERRAIN		=  7U;
const ShaderID	SHADER_NORMAL		=  8U;
const ShaderID	SHADER_BASIC		=  9U;
const ShaderID	SHADER_SHADOW		= 10U;
const ShaderID	SHADER_DUNGEON		= 11U;
const ShaderID	SHADER_SPRITE		= 12U;


const CameraID	CAMERA_PLAYER		=  0U;

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
	extern BufferID shdwBufferID;

	// Vector to store the opaque objects
	extern std::vector<ModelInstance> opaqueObjects;

	// Map to store the transparent objects
	extern std::multimap<float, ModelInstance> transparentObjects;
	
	Buffer& GetBuffer(BufferID id);

	void Cleanup();
	void CheckFrameBufferStatus(std::string bufferName);

	void AddBuffer(BufferID bufferKey);

	void InitFrameBuffer(BufferID bufferKey);
	void InitColorBuffer(BufferID bufferKey, GLuint textureType);
	void InitDepthBuffer(BufferID bufferKey);
	
	void BindTextureToFramebuffer(Buffer buffer, 
								  ImVec2& size, 
								  GLuint textureType);

	void SetupCaptureBuffer(Buffer buffer, int size);
	void BindDepthToFrameBuffer(Buffer buffer, ImVec2& size, bool multisampled);

	void BeginFrameBufferDrawing(GLuint frameBuffer, ImVec2& size);
	void CopyMSAABufferToDrawBuffer(GLuint msaaBuffer, 
									GLuint drawBuffer, 
									ImVec2& size);
	void EndFrameBufferDrawing(GLuint64 drawBuffer, ImVec2& size, ImVec2& pos);

	void RenderWorld();
	void RenderEnvMap(BufferID bufferID, int size,
					  glm::mat4& captureProjection, 
					  std::array<glm::mat4, 6>& captureViews);
	void RenderIrradianceMap(BufferID bufferID, int size,
							 glm::mat4& captureProjection, 
							 std::array<glm::mat4, 6>& captureViews);
	void RenderPrefilterMap(BufferID bufferID, int size,
							glm::mat4& captureProjection,
							std::array<glm::mat4, 6>& captureViews);
	void RenderBRDF(int size);
	void RenderDungeon();
}