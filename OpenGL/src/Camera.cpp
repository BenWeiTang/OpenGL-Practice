#include "Camera.h"

#include "GLFW/glfw3.h"

#include <iostream>

Camera::Camera()
	: m_Position(glm::vec3(0.0f, 0.0f, -3.0f))
	, m_Front(glm::vec3(0.0f, 0.0f, 1.0f))
	, m_Up(glm::vec3(0.0f, 1.0f, 0.0f))
	, m_Right(glm::normalize(glm::cross(m_Front, m_Up)))
	, m_WorldUp(glm::vec3(0.0f, 1.0f, 0.0f))
	, m_Window(glfwGetCurrentContext())
	, m_Yaw(YAW)
	, m_Pitch(PITCH)
	, m_Speed(SPEED)
	, m_MouseSensitivity(SENSITIVITY)
	, m_Zoom(ZOOM)
{
}

Camera::~Camera()
{
}

void Camera::Update(float deltaTime)
{
	if (m_Window == nullptr)
	{
		std::cout << "[Camera] member m_Window is nullptr." << std::endl;
		return;
	}

	if (glfwGetKey(m_Window, GLFW_KEY_W))
		MoveCamera(CameraMovement::FORWARD, deltaTime);
	else if (glfwGetKey(m_Window, GLFW_KEY_S))
		MoveCamera(CameraMovement::BACKWARD, deltaTime);
	else if (glfwGetKey(m_Window, GLFW_KEY_A))
		MoveCamera(CameraMovement::LEFT, deltaTime);
	else if (glfwGetKey(m_Window, GLFW_KEY_D))
		MoveCamera(CameraMovement::RIGHT, deltaTime);
}

void Camera::MoveCamera(CameraMovement direction, float deltaTime)
{
	float velocity = m_Speed * deltaTime;
	if (direction == CameraMovement::FORWARD)
		m_Position += m_Front * velocity;
	else if (direction == CameraMovement::BACKWARD)
		m_Position -= m_Front * velocity;
	else if (direction == CameraMovement::LEFT)
		m_Position -= m_Right * velocity;
	else if (direction == CameraMovement::RIGHT)
		m_Position += m_Right * velocity;
}
