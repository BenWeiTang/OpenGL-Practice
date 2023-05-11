#include "Camera.h"

#include "GLFW/glfw3.h"
#include <algorithm>

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
	, m_FirstMouse(true)
{
	glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	int width, height;
	glfwGetWindowSize(m_Window, &width, &height);
	m_MouseLastX = width / 2.0f;
	m_MouseLastY = height / 2.0f;
}

Camera::~Camera()
{
	glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Camera::Update(float deltaTime)
{
	if (m_Window == nullptr)
	{
		std::cout << "[Camera] member m_Window is nullptr." << std::endl;
		return;
	}

	UpdateCursorState();
	MoveCamera(deltaTime);
	RotateCamera();
}

void Camera::MoveCamera(float deltaTime)
{
	float velocity = m_Speed * deltaTime;
	// Forward
	if (glfwGetKey(m_Window, GLFW_KEY_W))
		m_Position += m_Front * velocity;

	// Backward
	else if (glfwGetKey(m_Window, GLFW_KEY_S))
		m_Position -= m_Front * velocity;
	
	// Left
	else if (glfwGetKey(m_Window, GLFW_KEY_A))
		m_Position -= m_Right * velocity;

	// Right
	else if (glfwGetKey(m_Window, GLFW_KEY_D))
		m_Position += m_Right * velocity;
}

void Camera::RotateCamera()
{
	// Get the offsets of x and y positions 
	double xPos, yPos;
	glfwGetCursorPos(m_Window, &xPos, &yPos);	
	if (m_FirstMouse)
	{
		m_MouseLastX = xPos;
		m_MouseLastY = yPos;
		m_FirstMouse = false;
	}
	float xOffset = xPos - m_MouseLastX;
	float yOffset = m_MouseLastY - yPos; // Invert y :P
	m_MouseLastX = xPos;
	m_MouseLastY = yPos;

	// Apply sensitivity before adding to yaw and pitch
	xOffset *= m_MouseSensitivity;
	yOffset *= m_MouseSensitivity;
	m_Yaw += xOffset;
	m_Pitch += yOffset;

	// Apply constraints to pitch to avoid gimbal lock 
	m_Pitch = std::clamp(m_Pitch, -89.0f, 89.0f);

	// Apply changes to Camera's front vector
	glm::vec3 direction;
	direction.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	direction.y = sin(glm::radians(m_Pitch));
	direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_Front = glm::normalize(direction);
	m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
	m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}

void Camera::UpdateCursorState()
{
	// Hit ESC to display cursor again (so user can exit)
	if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE))
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// Left click to go back to moving
	else if (glfwGetMouseButton(m_Window, 0) == GLFW_PRESS)
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
