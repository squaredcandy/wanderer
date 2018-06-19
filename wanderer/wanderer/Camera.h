#pragma once

#include <string>
#include <map>

#include <SDL/SDL.h>
#include <ImGui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/detail/type_vec.hpp>

struct CameraData
{
	float fov;
	float nearPlane;
	float farPlane;

	float translateSpeed;
	float rotateSpeed;

	glm::vec3 translation;
	// may need to change this to quaterition
	glm::vec3 rotation;	// yaw | pitch | roll

	// Vectors
	glm::vec3 forwardVector;
	glm::vec3 upVector;
	glm::vec3 rightVector;
	glm::vec3 worldUpVector;

	CameraData()
	{
		fov = 45.f;
		nearPlane = 0.1f;
		farPlane = 1000000.f;
		translateSpeed = 10;
		rotateSpeed = 10;

		translation = glm::vec3(-5, 5, 0);
		worldUpVector = glm::vec3(0.f, 1.f, 0.f);
		rotation[1] = -45.f;
	}

	~CameraData() = default;
};

using CameraID = unsigned int;

namespace Wanderer::Engine::Camera
{
	extern std::map<CameraID, CameraData> cameras;
	extern CameraID currentCamera;

	void AddCamera(CameraID cameraID);
	void CameraTranslation(float deltaTime);
	void CameraRotation(float deltaTime, ImVec2& size, ImVec2& pos);
	void UpdateCameraVectors();
	CameraData& GetCamera(CameraID key = currentCamera);
	glm::vec3& GetCameraPosition();
	glm::vec3 GetFaceCameraVector();
	glm::mat4 GetProjectionMatrix(ImVec2& size);
	glm::mat4 GetViewMatrix();
}