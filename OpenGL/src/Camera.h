#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// Default values for camera
const float YAW = 90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5F;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
	Camera();
	~Camera();
	void Update(float deltaTime);
	inline glm::mat4 GetViewMatrix() const { return glm::lookAt(m_Position, m_Position + m_Front, m_Up); }
	void SetCameraPosition(glm::vec3 newPosition);

private:
	glm::vec3 m_Position;
	glm::vec3 m_Front;
	glm::vec3 m_Up;
	glm::vec3 m_Right;
	const glm::vec3 m_WorldUp;

	class GLFWwindow* m_Window;

	float m_Yaw;
	float m_Pitch;
	float m_Speed;
	float m_MouseSensitivity;
	float m_MouseLastX;
	float m_MouseLastY;
	bool m_FirstMouse; // Flag to aviod huge mouse shake in the beginning

	void MoveCamera(float deltaTime);
	void RotateCamera();
	void UpdateCursorState();
};

