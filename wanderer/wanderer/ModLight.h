#pragma once

#include "Module.h"

#include <glm/glm.hpp>
#include <GL/gl3w.h>

class ModLight : public Module
{
private:
	float bufferSize;
public:
	bool drawLight;

	GLuint frameBuffer;
	GLuint cubemapID;
	
	glm::vec3 color;

	ModLight() : 
		bufferSize(1024.f),
		drawLight(true), 
		color ({ 500, 500, 500 })
	{
		// Setup Shadow Map
		glGenFramebuffers(1, &frameBuffer);
		glGenTextures(1, &cubemapID);

		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
		for (unsigned int i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			(int) bufferSize, (int) bufferSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubemapID, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	virtual ~ModLight()
	{
		glDeleteBuffers(1, &frameBuffer);
	};

	void Init() override
	{

	}

	void Update(float deltaTime) override
	{

	}

	void GUI() override
	{
		if (ImGui::TreeNode("Light Module"))
		{
			ImGui::Checkbox("Draw Light", &drawLight);
			ImGui::TreePop();
		}
	}

	const float GetDistance()
	{
		return std::fmaxf(color.x, std::fmaxf(color.y, color.z));
// 		return color.x > color.y ?
// 			color.x > color.z ? color.x : color.z :
// 			color.y > color.z ? color.y : color.z;
	}

	const float& GetSize() { return bufferSize; }
};