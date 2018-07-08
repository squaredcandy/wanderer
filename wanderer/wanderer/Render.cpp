#include "Render.h"

namespace Wanderer::Engine::Render
{
	GLuint sampleSize = 4;

	std::map<BufferID, Buffer> buffers;

	BufferID drawBufferID;
	BufferID msaaBufferID;
	BufferID shdwBufferID;

	std::vector<ModelInstance> opaqueObjects;
	std::multimap<float, ModelInstance> transparentObjects;

	void Cleanup()
	{
		for (auto& buffer : buffers)
		{
			glDeleteBuffers(1, &buffer.second.frame);
			glDeleteBuffers(1, &buffer.second.color);
			glDeleteBuffers(1, &buffer.second.depth);
		}
	}

	void CheckFrameBufferStatus(std::string bufferName)
	{
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR :: " << bufferName << 
			" :: Framebuffer is not complete!" << std::endl;
	}

	void AddBuffer(BufferID bufferKey)
	{
		if (buffers.find(bufferKey) != buffers.end())
		{
			printf("Overriding buffer %d", bufferKey);
		}
		Buffer newBuffer = Buffer();
		buffers[bufferKey] = newBuffer;
	}

	void InitFrameBuffer(BufferID bufferKey)
	{
		glGenFramebuffers(1, &buffers[bufferKey].frame);
		glBindFramebuffer(GL_FRAMEBUFFER, buffers[bufferKey].frame);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void InitColorBuffer(BufferID bufferKey, GLuint textureType)
	{
		glGenTextures(1, &buffers[bufferKey].color);
		glBindTexture(textureType, buffers[bufferKey].color);
		glBindTexture(textureType, 0);
	}

	void InitDepthBuffer(BufferID bufferKey)
	{
		glGenRenderbuffers(1, &buffers[bufferKey].depth);
		glBindRenderbuffer(GL_RENDERBUFFER, buffers[bufferKey].depth);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	void BindTextureToFramebuffer(Buffer buffer, 
								  ImVec2& size, 
								  GLuint textureType)
	{
		std::string bufferName;
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.frame);
		
		glBindTexture(textureType, buffer.color);
		if (textureType == GL_TEXTURE_2D)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
						 GLint(size.x), GLint(size.y),
						 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
								   GL_TEXTURE_2D, buffer.color, 0);
			bufferName = "TEXTUREBUFFER";
		}
		else if (textureType == GL_TEXTURE_2D_MULTISAMPLE)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sampleSize, 
									GL_RGB,
				(GLint) size.x, (GLint) size.y, GL_TRUE);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
								   GL_TEXTURE_2D_MULTISAMPLE, buffer.color, 0);
			bufferName = "MSAA TEXTUREBUFFER";
		}
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		CheckFrameBufferStatus(bufferName);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void BindDepthToFrameBuffer(Buffer buffer, ImVec2& size, bool multisampled)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.frame);
		glBindRenderbuffer(GL_RENDERBUFFER, buffer.depth);
		if (multisampled)
		{
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampleSize, 
											 GL_DEPTH24_STENCIL8,
											 (GLint) size.x, (GLint) size.y);
		}
		else
		{
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
				(GLint) size.x, (GLint) size.y);
		}
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
								  GL_RENDERBUFFER, buffer.depth);
		CheckFrameBufferStatus("DEPTHBUFFER");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void BindShadowToFrameBuffer(Buffer buffer, ImVec2& size, GLuint textureType)
	{
		std::string bufferName;
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.frame);

		glBindTexture(textureType, buffer.color);
		if (textureType == GL_TEXTURE_2D)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
						 GLint(size.x), GLint(size.y),
						 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
								   GL_TEXTURE_2D, buffer.color, 0);
			bufferName = "SHADOW TEXTUREBUFFER";
		}
		else if (textureType == GL_TEXTURE_2D_MULTISAMPLE)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sampleSize,
									GL_DEPTH_COMPONENT,
									(GLint) size.x, (GLint) size.y, GL_TRUE);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
								   GL_TEXTURE_2D_MULTISAMPLE, buffer.color, 0);
			bufferName = "MSAA SHADOW TEXTUREBUFFER";
		}

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		CheckFrameBufferStatus(bufferName);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void BeginFrameBufferDrawing(GLuint frameBuffer, ImVec2 & size)
	{
		glViewport(0, 0, (GLint) size.x, (GLint) size.y);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		// Move this to the camera
		ImVec4 clear_col = ImColor(15, 94, 156);
		glClearColor(clear_col.x, clear_col.y, clear_col.z, clear_col.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void CopyMSAABufferToDrawBuffer(GLuint msaaBuffer, GLuint drawBuffer, 
									ImVec2 & size)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawBuffer);
		glBlitFramebuffer(0, 0, (GLint) size.x, (GLint) size.y,
						  0, 0, (GLint) size.x, (GLint) size.y,
						  GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, 
						  GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}
	
	void EndFrameBufferDrawing(GLuint64 drawBuffer, ImVec2& size, ImVec2& pos)
	{
		glViewport(0, 0, 
				   (int) ImGui::GetIO().DisplaySize.x, 
				   (int) ImGui::GetIO().DisplaySize.y);

		glBindFramebuffer(GL_FRAMEBUFFER, (GLuint) drawBuffer);

		ImGui::GetWindowDrawList()->AddImageRounded(
			reinterpret_cast<GLvoid*>(drawBuffer), pos,
			ImVec2(pos.x + size.x, pos.y + size.y),
			ImVec2(0, 1), ImVec2(1, 0), 0xFFFFFFFF,
			ImGui::GetStyle().WindowRounding);
		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void CameraControls(ImVec2& pos, ImVec2& size)
	{
		static bool dragging = false;
		if (ImGui::IsWindowFocused() && ImGui::IsMouseHoveringWindow() || dragging)
		{
			SDL_SetWindowGrab(Engine::window, SDL_TRUE);
			float deltaTime = ImGui::GetIO().DeltaTime;
			Camera::CameraTranslation(deltaTime);

			if (ImGui::IsMouseDragging(1))
			{
				dragging = true;
				SDL_ShowCursor(0);
				Camera::CameraRotation(deltaTime, size, pos);
			}
			if (ImGui::IsMouseReleased(1))
			{
				dragging = false;
				SDL_ShowCursor(1);
				ImGui::ResetMouseDragDelta(1);
			}
		}
		else
		{
			SDL_SetWindowGrab(Engine::window, SDL_FALSE);
			SDL_ShowCursor(1);
			ImGui::ResetMouseDragDelta(1);
		}
	}

	void RenderWorld()
	{
		ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiSetCond_FirstUseEver);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Graphics Window", &Flag::showGraphicsWindow);

		auto pos = ImGui::GetWindowPos();
		auto size = ImGui::GetWindowSize();

		CameraControls(pos, size);

		Buffer drawBuffer = buffers[drawBufferID];
		Buffer msaaBuffer = buffers[msaaBufferID];
		Buffer shdwBuffer = buffers[shdwBufferID];

		BindShadowToFrameBuffer(shdwBuffer, size, GL_TEXTURE_2D);
		BeginFrameBufferDrawing(shdwBuffer.frame, size);

		float nearPlane = 1.f, farPlane = 7.5f;
		glm::mat4 lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, nearPlane, farPlane);

		auto dir = Debug::debugData.dLight.direction * glm::vec3(500);
		auto a = dir.y;
		dir.y = dir.z;
		dir.z = a;

		glm::mat4 lightView = glm::lookAt(dir,
										  glm::vec3(0.f, 0.f, 0.f),
										  glm::vec3(0.f, 1.f, 0.f));

		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		GLuint currentShader = SHADER_SHADOW;
		Shaders::SetCurrentShader(currentShader);
		Shaders::SetMat4("ProjectionView", lightSpaceMatrix);

		{
			using namespace Debug;
			Shaders::SetVec3("Wld_Eye_Pos", Camera::GetCameraPosition());
			Shaders::SetFloat("lodDistance", debugData.lodDist.data(), LOD_LENGTH);
			Shaders::SetFloat("tesLevels", debugData.tesLevel.data(), LOD_LENGTH);
			Shaders::SetFloat("heightFactor", debugData.heightFactor);
		}

		Shaders::SetInt("heightNoise", 0);

		auto heightmap = Textures::GetMaterial(CHUNK_TERRAIN);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, heightmap->textures[Material::MAP_HEIGHT]->textureID);

		auto planeMesh = Meshes::GetModel(MESH_PLANE);
		glBindVertexArray(planeMesh->VAO);

		glm::mat4 model = glm::translate(glm::mat4(), glm::vec3(3,0,0));
		model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1, 0, 0));
		model = glm::scale(model, glm::vec3(World::terrainScale));
		Shaders::SetMat4("Model", model);

		glDrawArrays(GL_PATCHES, 0, (GLuint) planeMesh->indices.size());

		glUseProgram(0);

		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//EndFrameBufferDrawing(shdwBuffer.frame, size, pos);
		}

 		{
 			BindTextureToFramebuffer(drawBuffer, size, GL_TEXTURE_2D);
 			BindDepthToFrameBuffer(drawBuffer, size, false);
 			if (sampleSize != 1)
 			{
 				BindTextureToFramebuffer(msaaBuffer, size, 
 										 GL_TEXTURE_2D_MULTISAMPLE);
 				BindDepthToFrameBuffer(msaaBuffer, size, true);
 				BeginFrameBufferDrawing(msaaBuffer.frame, size);
 			}
 			else
 			{
 				BeginFrameBufferDrawing(drawBuffer.frame, size);
 			}
 		}
 
 		Camera::UpdateCameraVectors();
 		glm::mat4 projection, view;
 
 		projection = Camera::GetProjectionMatrix(size);
 		view = Camera::GetViewMatrix();
 		//glCullFace(GL_BACK);
 
 		//auto planeMesh = Meshes::GetModel(MESH_PLANE);
 		glBindVertexArray(planeMesh->VAO);
 		glDepthFunc(GL_LESS);
 
 		currentShader = SHADER_TERRAIN;
 
 		Shaders::SetCurrentShader(currentShader);
 		Shaders::SetMat4("Projection", projection);
 		Shaders::SetMat4("View", view);
 
 		{
 			using namespace Debug;
 			Shaders::SetVec3("Wld_Eye_Pos", Camera::GetCameraPosition());
 			Shaders::SetFloat("heightFactor", debugData.heightFactor);
 			Shaders::SetFloat("lodDistance", debugData.lodDist.data(), LOD_LENGTH);
 			Shaders::SetFloat("tesLevels", debugData.tesLevel.data(), LOD_LENGTH);
 
 			Shaders::SetVec3("dLight.ambient", debugData.dLight.ambient);
 			Shaders::SetFloat("dLight.aIntensity", debugData.dLight.aIntensity);
 
 			Shaders::SetVec3("dLight.direction", 
 							 glm::normalize(debugData.dLight.direction));
 			Shaders::SetFloat("dLight.dIntensity", debugData.dLight.dIntensity);
 
 			Shaders::SetInt("chunkSize", World::chunkSize);
 			Shaders::SetBool("drawGridLines", World::drawGridLines);
 			Shaders::SetFloat("gridLineWidth", World::gridLineWidth);

			Shaders::SetMat4("lightSpaceMatrix", lightSpaceMatrix);
 		}
 
 		Shaders::SetInt("heightNoise", 0);
 
 		//auto heightmap = Textures::GetMaterial(CHUNK_TERRAIN);
 
 		glActiveTexture(GL_TEXTURE0);
 		glBindTexture(GL_TEXTURE_2D, heightmap->textures[Material::MAP_HEIGHT]->textureID);
 
 		model = glm::translate(glm::mat4(), glm::vec3(3,0,0));
 		model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1, 0, 0));
 		model = glm::scale(model, glm::vec3(World::terrainScale));
 		Shaders::SetMat4("Model", model);
 
 		glPatchParameteri(GL_PATCH_VERTICES, 3);
 		glDrawArrays(GL_PATCHES, 0, (GLuint) planeMesh->indices.size());
 
 		//auto log = Shaders::GetProgramError(currentShader);
 		//if(!log.empty())
 		//	std::cout << "Log: " << log.data() << std::endl;
 		
 		//currentShader = SHADER_NORMAL;
 		//Shaders::SetCurrentShader(currentShader);
 		//Shaders::SetMat4("Projection", projection);
 		//Shaders::SetMat4("View", view);
 		//
 		//{
 		//	using namespace Debug;
 		//	Shaders::SetVec3("Wld_Eye_Pos", Camera::GetCameraPosition());
 		//	Shaders::SetFloat("heightFactor", debugData.heightFactor);
 		//	Shaders::SetFloat("lodDistance", debugData.lodDist.data(), LOD_LENGTH);
 		//	Shaders::SetFloat("tesLevels", debugData.tesLevel.data(), LOD_LENGTH);
 		//}
 		//
 		//Shaders::SetInt("heightNoise", 0);
 		//
 		//model = glm::translate(glm::mat4(), glm::vec3(3, 0, 0));
 		//model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1, 0, 0));
 		//model = glm::scale(model, glm::vec3(World::terrainScale));
 		//Shaders::SetMat4("Model", model);
 		//
 		//glDrawArrays(GL_PATCHES, 0, (GLuint) planeMesh->indices.size());
 
 		currentShader = SHADER_BASIC;
 		Shaders::SetCurrentShader(currentShader);
 		Shaders::SetMat4("Projection", projection);
 		Shaders::SetMat4("View", view);
 
 		//auto dir = glm::normalize(Debug::debugData.dLight.direction) * glm::vec3(500);
 		//auto dir = Debug::debugData.dLight.direction * glm::vec3(500);
 		//auto a = dir.y;
 		//dir.y = dir.z;
 		//dir.z = a;
 
 		model = glm::translate(glm::mat4(), dir);
 		model = glm::scale(model, glm::vec3(10));
 		Shaders::SetMat4("Model", model);
 
 		auto sphereMesh = Meshes::GetModel(MESH_SPHERE);
 		glBindVertexArray(sphereMesh->VAO);
 		glDrawArrays(GL_TRIANGLES, 0, (GLuint) sphereMesh->vertices.size());
 		
 		glUseProgram(0);
 		{
 			glBindFramebuffer(GL_FRAMEBUFFER, 0);
 			if (sampleSize != 1)
 			{
 				CopyMSAABufferToDrawBuffer(msaaBuffer.frame, 
 										   drawBuffer.frame, 
 										   size);
 			}
 			EndFrameBufferDrawing(drawBuffer.frame, size, pos);
 		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void SetTexture(int texID, Material * mat, Material::TextureType texType)
	{
		glActiveTexture(GL_TEXTURE0 + texID);
		glBindTexture(GL_TEXTURE_2D, mat->textures[texType]->textureID);
	}

	void DrawModel(Mesh * mesh, GLint indices = 0)
	{
		glBindVertexArray(mesh->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
		if (indices == 0)
		{
			glDrawElements(GL_TRIANGLES, (GLuint) mesh->indices.size(), 
						   GL_UNSIGNED_INT, (GLvoid*) nullptr);
		}
		else
		{
			glDrawElementsInstanced(GL_TRIANGLES, (GLuint) mesh->indices.size(),
									GL_UNSIGNED_INT, (GLvoid*) nullptr, indices);
		}
	}

	void RenderDungeon()
	{
		ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiSetCond_FirstUseEver);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Graphics Window", &Flag::showGraphicsWindow);

		auto pos = ImGui::GetWindowPos();
		auto size = ImGui::GetWindowSize();

		CameraControls(pos, size);

		Buffer drawBuffer = buffers[drawBufferID];
		Buffer msaaBuffer = buffers[msaaBufferID];
		//Buffer shdwBuffer = buffers[shdwBufferID];

		// Bind Draw and MSAA buffers
		{
			BindTextureToFramebuffer(drawBuffer, size, GL_TEXTURE_2D);
			BindDepthToFrameBuffer(drawBuffer, size, false);
			if (sampleSize != 1)
			{
				BindTextureToFramebuffer(msaaBuffer, size,
										 GL_TEXTURE_2D_MULTISAMPLE);
				BindDepthToFrameBuffer(msaaBuffer, size, true);
				BeginFrameBufferDrawing(msaaBuffer.frame, size);
			}
			else
			{
				BeginFrameBufferDrawing(drawBuffer.frame, size);
			}
		}

		// Update Camera matrices
		Camera::UpdateCameraVectors();
		glm::mat4 projection = Camera::GetProjectionMatrix(size);
		glm::mat4 view = Camera::GetViewMatrix();

		// Bind Mesh vao
		auto floorMesh = Meshes::GetModel(MESH_PLANE);
		auto wallMesh = Meshes::GetModel(MESH_WALL);
		auto doorArchMesh = Meshes::GetModel(MESH_DOORARCH);
		auto doorCapMesh = Meshes::GetModel(MESH_DOORCAP);
		auto gateMesh = Meshes::GetModel(MESH_GATE);

		auto wallTex = Textures::GetMaterial(TEX_STONEWALL);
		auto floorTex = Textures::GetMaterial(TEX_COBBLE);
		//auto archTex = Textures::GetMaterial(TEX_ARCH);
		auto steelRustTex = Textures::GetMaterial(TEX_STEELRUST);

		auto lvl = Dungeon::GetCurrentLevel();
		auto wallCount = std::get<1>(lvl.vbos["Wall"]);
		auto floorCount = std::get<1>(lvl.vbos["Floor"]);
		auto doorArchCount = std::get<1>(lvl.vbos["DoorArch"]);
		auto doorCapCount = std::get<1>(lvl.vbos["DoorArchCap"]);
		auto gateCount = std::get<1>(lvl.vbos["Gate"]);

		// Depth Test Function
		glDepthFunc(GL_LESS);

		// Bind Current Shader
		auto currentShader = SHADER_DUNGEON;
		Shaders::SetCurrentShader(currentShader);

		Shaders::SetMat4("Projection", projection);
		Shaders::SetMat4("View", view);
		
		Shaders::SetInt("textureSample", 0);

		SetTexture(0, floorTex, Material::MAP_DIFFUSE);
		DrawModel(floorMesh, floorCount);

		SetTexture(0, wallTex, Material::MAP_DIFFUSE);
		DrawModel(wallMesh, wallCount);
		DrawModel(doorArchMesh, doorArchCount);
		DrawModel(doorCapMesh, doorCapCount);

		SetTexture(0, steelRustTex, Material::MAP_DIFFUSE);
		DrawModel(gateMesh, gateCount);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		static float i = 0;
		i += ImGui::GetIO().DeltaTime / 10;
		auto gateVBO = std::get<0>(lvl.vbos["Gate"]);
		static glm::mat4 a;
		a = glm::translate(a, glm::vec3(0, i, 0));
		glNamedBufferSubData(gateVBO, 0, sizeof(glm::mat4), (GLvoid*) &a);

		glBindVertexArray(0);
		glUseProgram(0);

		// End Draw Buffers
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			if (sampleSize != 1)
			{
				CopyMSAABufferToDrawBuffer(msaaBuffer.frame,
										   drawBuffer.frame,
										   size);
			}
			EndFrameBufferDrawing(drawBuffer.frame, size, pos);
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}
}