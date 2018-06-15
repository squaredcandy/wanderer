#include "Render.h"

namespace Wanderer::Engine::Render
{
	GLuint sampleSize = 1;

	std::map<BufferID, Buffer> buffers;

	BufferID drawBufferID;
	BufferID msaaBufferID;

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
						 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
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

	void BeginFrameBufferDrawing(GLuint frameBuffer, ImVec2 & size)
	{
		glViewport(0, 0, (int) size.x, (int) size.y);
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

	void EndFrameBufferDrawing(GLuint drawBuffer, ImVec2& size, ImVec2& pos)
	{
		glViewport(0, 0, 
				   (int) ImGui::GetIO().DisplaySize.x, 
				   (int) ImGui::GetIO().DisplaySize.y);

		glBindFramebuffer(GL_FRAMEBUFFER, drawBuffer);

		ImGui::GetWindowDrawList()->AddImageRounded(
			(void*)drawBuffer, pos,
			ImVec2(pos.x + size.x, pos.y + size.y),
			ImVec2(0, 1), ImVec2(1, 0), 0xFFFFFFFF,
			ImGui::GetStyle().WindowRounding);
		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void RenderWorld()
	{
		ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiSetCond_FirstUseEver);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Graphics Window", &Flag::showGraphicsWindow);

		auto pos = ImGui::GetWindowPos();
		auto size = ImGui::GetWindowSize();

		if (ImGui::IsWindowFocused() && ImGui::IsMousePosValid())
		{
			ImGui::SetKeyboardFocusHere();
			float deltaTime = ImGui::GetIO().DeltaTime;
			Camera::CameraTranslation(deltaTime);

			if (ImGui::IsMouseDragging(1))
			{
				Camera::CameraRotation(deltaTime, size, pos);
			}
			if (ImGui::IsMouseReleased(1))
			{
				//SDL_ShowCursor(1);
				ImGui::ResetMouseDragDelta(1);
			}
		}
		else
		{
			//SDL_ShowCursor(1);
			ImGui::ResetMouseDragDelta(1);
		}

		Buffer drawBuffer = buffers[drawBufferID];
		Buffer msaaBuffer = buffers[msaaBufferID];

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
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		Camera::UpdateCameraVectors();

		glm::mat4 projection, view, model;
		projection = Camera::GetProjectionMatrix(size);
		view = Camera::GetViewMatrix();
		glCullFace(GL_BACK);

		auto planeMesh = Meshes::GetModel(MESH_PLANE);
		glBindVertexArray(planeMesh->VAO);
		glDepthFunc(GL_LESS);

		Shaders::SetCurrentShader(SHADER_TERRAIN);
		Shaders::SetMat4("Projection", projection);
		Shaders::SetMat4("View", view);
		Shaders::SetInt("heightNoise", 0);
		Shaders::SetFloat("tVal", Debug::debugData.tVal);

		auto heightmap = Textures::GetMaterial(CHUNK_TERRAIN);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, heightmap->textures[Material::MAP_HEIGHT]->textureID);

		model = glm::translate(glm::mat4(), glm::vec3(3,0,0));
		model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1, 0, 0));
		model = glm::scale(model, glm::vec3(1));
		Shaders::SetMat4("Model", model);
		glDrawArrays(GL_TRIANGLES, 0, (GLuint) planeMesh->indices.size());
		
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


}