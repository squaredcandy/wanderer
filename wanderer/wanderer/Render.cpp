#include "Render.h"

namespace Wanderer::Engine::Render
{
	const int MAX_LIGHTS = 32;

	GLuint sampleSize = 1;

	std::map<BufferID, Buffer> buffers;

	BufferID drawBufferID;
	BufferID msaaBufferID;
	BufferID shdwBufferID;

	std::vector<ModelInstance> opaqueObjects;
	std::multimap<float, ModelInstance> transparentObjects;

	void RenderCube()
	{
		static unsigned int cubeVAO = 0;
		static unsigned int cubeVBO = 0;
		// initialize (if necessary)
		if (cubeVAO == 0)
		{
			float vertices[] = {
				// back face
				-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
				1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
				1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
				-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left
				// front face
				-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
				1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom-right
				1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
				1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
				-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // top-left
				-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
				// left face
				-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
				-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
				-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-right
				-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
				// right face
				1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
				1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
				1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right         
				1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
				1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
				1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left     
				// bottom face
				-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
				1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
				1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
				1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
				-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
				-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
				// top face
				-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
				1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
				1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right     
				1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
				-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
				-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f  // bottom-left
			};
			glGenVertexArrays(1, &cubeVAO);
			glGenBuffers(1, &cubeVBO);
			// fill buffer
			glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			// link vertex attributes
			glBindVertexArray(cubeVAO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		// render Cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		//glDeleteVertexArrays(1, &cubeVAO);
		//glDeleteBuffers(1, &cubeVBO);
	}

	void RenderQuad()
	{
		static unsigned int quadVAO = 0;
		static unsigned int quadVBO = 0;
		if (quadVAO == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		//glDeleteVertexArrays(1, &quadVAO);
		//glDeleteBuffers(1, &quadVBO);
	}

	Buffer& GetBuffer(BufferID id)
	{
		return buffers[id];
	}

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

	void SetupCaptureBuffer(Buffer buffer,
								 int size)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.frame);
		glBindRenderbuffer(GL_RENDERBUFFER, buffer.depth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
								  GL_RENDERBUFFER, buffer.depth);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		CheckFrameBufferStatus("CAPTUREBUFFER");
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

	void BindDirectionShadowToFrameBuffer(Buffer buffer, ImVec2& size, GLuint textureType)
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
		ImVec4 clear_col;
		if (Flag::debugMode) clear_col = ImColor(15, 94, 156);
		else clear_col = ImColor();
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

		BindDirectionShadowToFrameBuffer(shdwBuffer, size, GL_TEXTURE_2D);
		BeginFrameBufferDrawing(shdwBuffer.frame, size);

		float nearPlane = 1.f, farPlane = 7.5f;
		glm::mat4 lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, nearPlane, farPlane);

		auto dir = Debug::data.dLight.direction * glm::vec3(500);
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
			Shaders::SetFloat("lodDistance", data.lodDist.data(), LOD_LENGTH);
			Shaders::SetFloat("tesLevels", data.tesLevel.data(), LOD_LENGTH);
			Shaders::SetFloat("heightFactor", data.heightFactor);
		}

		Shaders::SetInt("heightNoise", 0);

		auto heightmap = Textures::GetMaterial("Terrain");

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, heightmap->textures[Material::MAP_HEIGHT].textureID);

		auto planeMesh = Meshes::GetModel(MESH_FLOOR);
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
 			Shaders::SetFloat("heightFactor", data.heightFactor);
 			Shaders::SetFloat("lodDistance", data.lodDist.data(), LOD_LENGTH);
 			Shaders::SetFloat("tesLevels", data.tesLevel.data(), LOD_LENGTH);
 
 			Shaders::SetVec3("dLight.ambient", data.dLight.ambient);
 			Shaders::SetFloat("dLight.aIntensity", data.dLight.aIntensity);
 
 			Shaders::SetVec3("dLight.direction", 
 							 glm::normalize(data.dLight.direction));
 			Shaders::SetFloat("dLight.dIntensity", data.dLight.dIntensity);
 
 			Shaders::SetInt("chunkSize", World::chunkSize);
 			Shaders::SetBool("drawGridLines", World::drawGridLines);
 			Shaders::SetFloat("gridLineWidth", World::gridLineWidth);

			Shaders::SetMat4("lightSpaceMatrix", lightSpaceMatrix);
 		}
 
 		Shaders::SetInt("heightNoise", 0);
 
 		//auto heightmap = Textures::GetMaterial(CHUNK_TERRAIN);
 
 		glActiveTexture(GL_TEXTURE0);
 		glBindTexture(GL_TEXTURE_2D, heightmap->textures[Material::MAP_HEIGHT].textureID);
 
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
 		//	Shaders::SetFloat("heightFactor", data.heightFactor);
 		//	Shaders::SetFloat("lodDistance", data.lodDist.data(), LOD_LENGTH);
 		//	Shaders::SetFloat("tesLevels", data.tesLevel.data(), LOD_LENGTH);
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
 
 		//auto dir = glm::normalize(Debug::data.dLight.direction) * glm::vec3(500);
 		//auto dir = Debug::data.dLight.direction * glm::vec3(500);
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

	void RenderEnvMap(BufferID bufferID, int size, 
					  glm::mat4& captureProjection, 
					  std::array<glm::mat4, 6>& captureViews)
	{
		// Change Depth Function
		glDepthFunc(GL_LEQUAL);

		auto currentShader = SHADER_EQUTOCUBE;
		auto capture = Textures::GetMaterial("Capture");
		auto buffer = buffers[bufferID];

		Shaders::SetCurrentShader(currentShader);
		Shaders::SetInt("equirectangularMap", 0);
		Shaders::SetMat4("projection", captureProjection);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, capture->textures[Material::MAP_ENVMAP].textureID);

		glViewport(0, 0, size, size);
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.frame);
		// Render each side
		for (auto i = 0; i < 6; ++i)
		{
			Shaders::SetMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
								   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
								   capture->textures[Material::MAP_CUBEMAP].textureID, 0);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			RenderCube();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Generate Cubemap Mipmaps
		glBindTexture(GL_TEXTURE_CUBE_MAP, capture->textures[Material::MAP_CUBEMAP].textureID);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	void RenderIrradianceMap(BufferID bufferID, int size,
							 glm::mat4& captureProjection, 
							 std::array<glm::mat4, 6>& captureViews)
	{
		auto currentShader = SHADER_IRRADIANCE;
		auto capture = Textures::GetMaterial("Capture");
		auto irradiance = Textures::GetMaterial("Irradiance");
		auto buffer = buffers[bufferID];

		Shaders::SetCurrentShader(currentShader);
		Shaders::SetInt("environmentMap", 0);
		Shaders::SetMat4("projection", captureProjection);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, capture->textures[Material::MAP_CUBEMAP].textureID);

		glViewport(0, 0, size, size); 
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.frame);
		for (unsigned int i = 0; i < 6; ++i)
		{
			Shaders::SetMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
								   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
								   irradiance->textures[Material::MAP_CUBEMAP].textureID, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			RenderCube();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void RenderPrefilterMap(BufferID bufferID, int size,
							glm::mat4& captureProjection,
							std::array<glm::mat4, 6>& captureViews)
	{
		auto currentShader = SHADER_PREFILTER;
		auto capture = Textures::GetMaterial("Capture");
		auto prefilter = Textures::GetMaterial("Prefilter");
		auto buffer = buffers[bufferID];

		Shaders::SetCurrentShader(currentShader);
		Shaders::SetInt("environmentMap", 0);
		Shaders::SetMat4("projection", captureProjection);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, capture->textures[Material::MAP_CUBEMAP].textureID);

		glBindFramebuffer(GL_FRAMEBUFFER, buffer.frame);
		unsigned int maxMipLevels = 5;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			// reisze framebuffer according to mip-level size.
			auto mipWidth = (unsigned int)(128.0 * std::pow(0.5, mip));
			auto mipHeight = (unsigned int)(128.0 * std::pow(0.5, mip));
			glBindRenderbuffer(GL_RENDERBUFFER, buffer.depth);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float) mip / (float) (maxMipLevels - 1);
			Shaders::SetFloat("roughness", roughness);
			for (unsigned int i = 0; i < 6; ++i)
			{
				Shaders::SetMat4("view", captureViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
									   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
									   prefilter->textures[Material::MAP_CUBEMAP].textureID, 0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				RenderCube();
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void RenderBRDF(int size)
	{
		auto currentShader = SHADER_BRDF;
		Shaders::SetCurrentShader(currentShader);

		glViewport(0, 0, size, size);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		RenderQuad();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void SetTexture(int texID, GLuint glTexType, Material * mat, Material::TextureType texType)
	{
		glActiveTexture(GL_TEXTURE0 + texID);
		glBindTexture(glTexType, mat->textures[texType].textureID);
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

	void DrawModel(DungeonModelData& model)
	{
		if (model.length == 0) return;
		glBindVertexArray(model.mesh->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.mesh->EBO);
		glDrawElementsInstanced(GL_TRIANGLES, (GLuint) model.mesh->indices.size(), 
								GL_UNSIGNED_INT, (GLvoid*) nullptr, model.length);
	}

	void RenderShadows(CameraData& camera, Entity * light)
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto& l = light->GetMod<ModLight>();
		if (!l.drawLight || glm::distance(camera.translation, 
				light->GetTransform().GetTranslate()) > camera.lightDistance)
		{
			return;
		}
		//glm::vec2 size{ 1024, 1024 };
		glm::mat4 shadowProjection = glm::perspective(glm::radians(90.f), 
													   l.GetSize() / l.GetSize(),
													  camera.nearPlane, 
													  250.f);
		glm::vec3 lightPos = light->GetTransform().GetTranslate();
		std::vector<glm::mat4> shadowTransforms
		{
			shadowProjection * glm::lookAt(lightPos, lightPos + 
			glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			shadowProjection * glm::lookAt(lightPos, lightPos + 
			glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			shadowProjection * glm::lookAt(lightPos, lightPos + 
			glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
			shadowProjection * glm::lookAt(lightPos, lightPos + 
			glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
			shadowProjection * glm::lookAt(lightPos, lightPos + 
			glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			shadowProjection * glm::lookAt(lightPos, lightPos + 
			glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
		};
		glDisable(GL_CULL_FACE);
		glViewport(0, 0, (int) l.GetSize(), (int) l.GetSize());
		glBindFramebuffer(GL_FRAMEBUFFER, l.frameBuffer);
		glClear(GL_DEPTH_BUFFER_BIT);
		Shaders::SetCurrentShader(SHADER_SHADOW);
		for (unsigned int i = 0; i < shadowTransforms.size(); ++i)
		{
			Shaders::SetMat4("shadowMatrices[" + std::to_string(i) + "]", 
							 shadowTransforms[i]);
		}
		Shaders::SetFloat("far_plane", l.GetDistance());
		Shaders::SetVec3("lightPos", lightPos);

		// Render Scene
		auto& lvl = Dungeon::GetCurrentLevel();
		DrawModel(lvl.models[TileID::Floor]);
		DrawModel(lvl.models[TileID::Upstairs]);
		DrawModel(lvl.models[TileID::Downstairs]);

		DrawModel(lvl.models[TileID::Wall]);
		DrawModel(lvl.models[TileID::Arch]);
		DrawModel(lvl.models[TileID::ArchCap]);

		DrawModel(lvl.models[TileID::Gate]);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_CULL_FACE);
	}

	void RenderDungeon()
	{
		ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiSetCond_FirstUseEver);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Graphics Window", &Flag::showGraphicsWindow);

		auto pos = ImGui::GetWindowPos();
		auto size = ImGui::GetWindowSize();

		CameraControls(pos, size);

		// Render Shadows
		Camera::UpdateCameraVectors();

		auto lights = Entities::GetAll<ModLight>();
		for (auto light : lights)
		{
			RenderShadows(Camera::GetCamera(), light);
		}

		Buffer drawBuffer = buffers[drawBufferID];
		Buffer msaaBuffer = buffers[msaaBufferID];

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
		glm::mat4 projection = Camera::GetProjectionMatrix(size);
		glm::mat4 view = Camera::GetViewMatrix();
		glm::vec3 camPos = Camera::GetCameraPosition();

		// IBL data
// 		auto capture = Textures::GetMaterial(TEX_CAPTURE);
// 		auto irradiance = Textures::GetMaterial(TEX_IRRADIANCE);
// 		auto prefilter = Textures::GetMaterial(TEX_PREFILTER);

		// Textures
		auto wallTex = Textures::GetMaterial("Stonewall");
		auto floorTex = Textures::GetMaterial("Cobble2");
		auto gateTex = Textures::GetMaterial("SteelBattered");
		auto slashTex = Textures::GetMaterial("Slash");

		auto& lvl = Dungeon::GetCurrentLevel();

		// Depth Test Function
 		glDepthFunc(GL_LESS);
 
		// Bind Current Shader
		Shaders::SetCurrentShader(SHADER_PBR);

		Shaders::SetMat4("projection", projection);
		Shaders::SetMat4("view", view);
		Shaders::SetVec3("camPos", camPos);

		//Shaders::SetFloat("heightScale", Debug::data.heightFactor);

		int i = 0;
		for (auto& light : lights)
		{
			auto& mod = light->GetMod<ModLight>();
			if (!mod.drawLight)continue;
			std::string idx = "[" + std::to_string(i) + "]";
			Shaders::SetVec3("lightPositions" + idx, light->GetTransform().GetTranslate());
			Shaders::SetVec3("lightColors" + idx, mod.color);
			Shaders::SetFloat("far_plane" + idx, mod.GetDistance());
			glActiveTexture(GL_TEXTURE9 + i);
			glBindTexture(GL_TEXTURE_CUBE_MAP, mod.cubemapID);
			if (++i > MAX_LIGHTS) break;
		}
		Shaders::SetInt("lightLength", i);
		Shaders::SetFloat("bias", Debug::data.bias);
		Shaders::SetInt("samples", Debug::data.samples);

// 		SetTexture(0, GL_TEXTURE_CUBE_MAP, irradiance, Material::MAP_CUBEMAP);
// 		SetTexture(1, GL_TEXTURE_CUBE_MAP, prefilter, Material::MAP_CUBEMAP);
// 		SetTexture(2, GL_TEXTURE_2D, prefilter, Material::MAP_DIFFUSE);

		SetTexture(3, GL_TEXTURE_2D, floorTex, Material::MAP_DIFFUSE);
		SetTexture(4, GL_TEXTURE_2D, floorTex, Material::MAP_NORMAL);
		SetTexture(5, GL_TEXTURE_2D, floorTex, Material::MAP_METALLIC);
		SetTexture(6, GL_TEXTURE_2D, floorTex, Material::MAP_ROUGHNESS);
		SetTexture(7, GL_TEXTURE_2D, floorTex, Material::MAP_AO);
		SetTexture(8, GL_TEXTURE_2D, floorTex, Material::MAP_HEIGHT);
		DrawModel(lvl.models[TileID::Floor]);
		DrawModel(lvl.models[TileID::Upstairs]);
		DrawModel(lvl.models[TileID::Downstairs]);

		SetTexture(3, GL_TEXTURE_2D, wallTex, Material::MAP_DIFFUSE);
		SetTexture(4, GL_TEXTURE_2D, wallTex, Material::MAP_NORMAL);
		SetTexture(5, GL_TEXTURE_2D, wallTex, Material::MAP_METALLIC);
		SetTexture(6, GL_TEXTURE_2D, wallTex, Material::MAP_ROUGHNESS);
		SetTexture(7, GL_TEXTURE_2D, wallTex, Material::MAP_AO);
		SetTexture(8, GL_TEXTURE_2D, wallTex, Material::MAP_HEIGHT);
		DrawModel(lvl.models[TileID::Wall]);
		DrawModel(lvl.models[TileID::Arch]);
		DrawModel(lvl.models[TileID::ArchCap]);

		SetTexture(3, GL_TEXTURE_2D, gateTex, Material::MAP_DIFFUSE);
		SetTexture(4, GL_TEXTURE_2D, gateTex, Material::MAP_NORMAL);
		SetTexture(5, GL_TEXTURE_2D, gateTex, Material::MAP_METALLIC);
		SetTexture(6, GL_TEXTURE_2D, gateTex, Material::MAP_ROUGHNESS);
		SetTexture(7, GL_TEXTURE_2D, gateTex, Material::MAP_AO);
		SetTexture(8, GL_TEXTURE_2D, gateTex, Material::MAP_HEIGHT);
		DrawModel(lvl.models[TileID::Gate]);

		if (Flag::debugMode)
		{
			Shaders::SetCurrentShader(SHADER_BASIC);
			Shaders::SetMat4("Projection", projection);
			Shaders::SetMat4("View", view);

			for (auto& light : lights)
			{
				auto position = light->GetTransform().GetTranslate();
				auto& mod = light->GetMod<ModLight>();
				auto model = glm::translate(glm::mat4(), position);
				model = glm::scale(model, glm::vec3(0.1f));

				Shaders::SetMat4("Model", model);
				Shaders::SetVec3("Color", mod.color);
				auto sphere = Meshes::GetModel(MESH_SPHERE);
				DrawModel(sphere);
			}
		}

		glDisable(GL_CULL_FACE);
		Shaders::SetCurrentShader(SHADER_SPRITE);
		Shaders::SetInt("sprite", 0);
		Shaders::SetFloat("width", 4);
		Shaders::SetFloat("height", 4);
		Shaders::SetInt("x", Debug::data.x);
		Shaders::SetInt("y", Debug::data.y);
		Shaders::SetMat4("Projection", projection);
		Shaders::SetMat4("View", view);
		auto model = glm::translate(glm::mat4(), glm::vec3(5,5,5));
		model = glm::scale(model, glm::vec3(1.77f, 1, 1));
		model = glm::scale(model, glm::vec3(0.5f));
		Shaders::SetMat4("Model", model);
		SetTexture(0, GL_TEXTURE_2D, slashTex, Material::MAP_DIFFUSE);
		auto plane = Meshes::GetModel(MESH_SPRITE);
		DrawModel(plane);
		glEnable(GL_CULL_FACE);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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