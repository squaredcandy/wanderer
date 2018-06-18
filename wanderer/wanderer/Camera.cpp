#include "Camera.h"


namespace Wanderer::Engine::Camera
{
	std::map<CameraID, CameraData> cameras;
	CameraID currentCamera;

	void AddCamera(CameraID cameraID)
	{
		cameras[cameraID] = CameraData();
		currentCamera = cameraID;
	}

	void CameraTranslation(float deltaTime)
	{
		auto& cam = cameras[currentCamera];

		float movSpeed = cam.translateSpeed * deltaTime;
		if (ImGui::GetIO().KeysDown[SDL_SCANCODE_W]) cam.translation += cam.forwardVector * movSpeed;
		if (ImGui::GetIO().KeysDown[SDL_SCANCODE_S]) cam.translation -= cam.forwardVector * movSpeed;
		if (ImGui::GetIO().KeysDown[SDL_SCANCODE_D]) cam.translation += cam.rightVector * movSpeed;
		if (ImGui::GetIO().KeysDown[SDL_SCANCODE_A]) cam.translation -= cam.rightVector * movSpeed;
		if (ImGui::GetIO().KeysDown[SDL_SCANCODE_Q]) cam.translation += cam.upVector * movSpeed; 
		if (ImGui::GetIO().KeysDown[SDL_SCANCODE_E]) cam.translation -= cam.upVector * movSpeed;
	}

	void CameraRotation(float deltaTime, ImVec2& size, ImVec2& pos)
	{
		auto& cam = cameras[currentCamera];
		//if(SDL_ShowCursor(-1)) SDL_ShowCursor(0);
		auto clickPos = ImGui::GetIO().MouseClickedPos[1];
		auto x0State = clickPos[0] > pos[0];
		auto x1State = clickPos[0] < pos[0] + size[0];
		auto y0State = clickPos[1] > pos[1];
		auto y1State = clickPos[1] < pos[1] + size[1];

		if (!x0State)
		{
			SDL_WarpMouseInWindow(SDL_GetWindowFromID(0), int(clickPos[0] + size[0] + 1), int(clickPos[1]));
			ImGui::ResetMouseDragDelta(1);
		}
		if (!x1State)
		{
			SDL_WarpMouseInWindow(SDL_GetWindowFromID(0), int(clickPos[0] - size[0] - 1), int(clickPos[1]));
			ImGui::ResetMouseDragDelta(1);
		}
		if (!y0State)
		{
			SDL_WarpMouseInWindow(SDL_GetWindowFromID(0), int(clickPos[0]), int(clickPos[1] + size[1] + 1));
			ImGui::ResetMouseDragDelta(1);
		}
		if (!y1State)
		{
			SDL_WarpMouseInWindow(SDL_GetWindowFromID(0), int(clickPos[0]), int(clickPos[1] - size[1] - 1));
			ImGui::ResetMouseDragDelta(1);
		}

		if (x0State && x1State && y0State && y1State)
		{
			auto dragDelta = ImGui::GetMouseDragDelta(1);
			auto rotSpeed = cam.rotateSpeed * deltaTime;
			auto delta = dragDelta;
			ImGui::ResetMouseDragDelta(1);
			cam.rotation[0] += delta.x * rotSpeed;
			cam.rotation[1] = glm::clamp(cam.rotation[1] - (delta.y * rotSpeed), -89.9f, 89.9f);

			UpdateCameraVectors();
		}
	}

	void UpdateCameraVectors()
	{
		auto& cam = cameras[currentCamera];

		auto yawRad	   = glm::radians(cam.rotation[0]);
		auto pitchRad  = glm::radians(cam.rotation[1]);
		auto cYawRad   = cosf(yawRad);
		auto cPitchRad = cosf(pitchRad);
		auto sYawRad   = sinf(yawRad);
		auto sPitchRad = sinf(pitchRad);

		auto forward = glm::vec3(cYawRad * cPitchRad, sPitchRad, sYawRad * cPitchRad);
		cam.forwardVector = glm::normalize(forward);
		cam.rightVector = glm::normalize(glm::cross(cam.forwardVector, cam.worldUpVector));
		cam.upVector = glm::normalize(glm::cross(cam.rightVector, cam.forwardVector));
	}

	glm::vec3& GetCameraPosition()
	{
		return cameras[currentCamera].translation;
	}

	glm::vec3 GetFaceCameraVector()
	{
		return glm::vec3(0, cameras[currentCamera].rotation.x + 90.f, 0);
	}

	glm::mat4 GetProjectionMatrix(ImVec2& size)
	{
		auto& cam = cameras[currentCamera];
		return glm::perspective(cam.fov, size.x / size.y, cam.nearPlane, cam.farPlane);
	}

	glm::mat4 GetViewMatrix()
	{
		auto& cam = cameras[currentCamera];
		return glm::lookAt(cam.translation, cam.translation + cam.forwardVector, cam.upVector);
	}
}