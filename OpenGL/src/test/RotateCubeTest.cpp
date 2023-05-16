#include "RotateCubeTest.h"

#include "Renderer.h"

#include "glm/gtc/matrix_transform.hpp"
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"

#include <iostream>

namespace test
{
	RotateCubeTest::RotateCubeTest()
		: m_Model(glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 1.0f, 0.0f))),
		m_Proj(glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 100.0f)),
		m_CubePositions{
			glm::vec3(0.0f,  0.0f,  0.0f),
			glm::vec3(2.0f,  5.0f, -15.0f),
			glm::vec3(-1.5f, -2.2f, -2.5f),
			glm::vec3(-3.8f, -2.0f, -12.3f),
			glm::vec3(2.4f, -0.4f, -3.5f),
			glm::vec3(-1.7f,  3.0f, -7.5f),
			glm::vec3(1.3f, -2.0f, -2.5f),
			glm::vec3(1.5f,  2.0f, -2.5f),
			glm::vec3(1.5f,  0.2f, -1.5f),
			glm::vec3(-1.3f,  1.0f, -1.5f) },
		m_CameraPos(glm::vec3(0.0f, 0.0f, 3.0f)),
		m_CameraTarget(glm::vec3(0.0f, 0.0f, 0.0f)),
		m_CameraDir(glm::normalize(m_CameraPos - m_CameraTarget)), // In OpenGL, the look direction is reversed
		m_CameraRight(glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_CameraDir))), // Using a temp world up vector to get right vector, (Again, the dir vector is a bit weird)
		m_CameraUp(glm::normalize(glm::cross(m_CameraDir, m_CameraUp))),
		m_View(glm::lookAt(m_CameraPos, m_CameraTarget, m_CameraUp)), // (0, 0, 3), (0, 0, 0), and (0, 1, 0)
		m_Camera(std::make_unique<Camera>())
	{
		float pos[8][3] =
		{
			{-0.5f,  0.5f,  0.5f}, // 0 top left front 
			{ 0.5f,  0.5f,  0.5f}, // 1 top right front
			{ 0.5f, -0.5f,  0.5f}, // 2 buttom right front
			{-0.5f, -0.5f,  0.5f}, // 3 button left front

			{-0.5f,  0.5f, -0.5f}, // 4 top left back
			{ 0.5f,  0.5f, -0.5f}, // 5 top right back
			{ 0.5f, -0.5f, -0.5f}, // 6 bottom right back
			{-0.5f, -0.5f, -0.5f}  // 7 bottom left back
		};

		float vertices[] = {
		 /* position                             UV coord */
			// Front
			 pos[0][0],  pos[0][1],  pos[0][2],  0.0f, 1.0f, // 0
			 pos[1][0],  pos[1][1],  pos[1][2],  1.0f, 1.0f, // 1
			 pos[2][0],  pos[2][1],  pos[2][2],  1.0f, 0.0f, // 2
			 pos[3][0],  pos[3][1],  pos[3][2],  0.0f, 0.0f, // 3

			// Back
			 pos[4][0],  pos[4][1], pos[4][2],  1.0f, 1.0f, // 4
			 pos[5][0],  pos[5][1], pos[5][2],  0.0f, 1.0f, // 5
			 pos[6][0],  pos[6][1], pos[6][2],  0.0f, 0.0f, // 6
			 pos[7][0],  pos[7][1], pos[7][2],  1.0f, 0.0f, // 7

			// Top
			 pos[0][0],  pos[0][1], pos[0][2],  1.0f, 1.0f, // 8
			 pos[1][0],  pos[1][1], pos[1][2],  0.0f, 1.0f, // 9
			 pos[5][0],  pos[5][1], pos[5][2],  0.0f, 0.0f, // 10
			 pos[4][0],  pos[4][1], pos[4][2],  1.0f, 0.0f, // 11

			// Bottom
			 pos[3][0],  pos[3][1], pos[3][2],  0.0f, 1.0f, // 12
			 pos[2][0],  pos[2][1], pos[2][2],  1.0f, 1.0f, // 13
			 pos[6][0],  pos[6][1], pos[6][2],  1.0f, 0.0f, // 14
			 pos[7][0],  pos[7][1], pos[7][2],  0.0f, 0.0f, // 15

			// Right
			 pos[1][0],  pos[1][1], pos[1][2],  0.0f, 1.0f, // 16
			 pos[5][0],  pos[5][1], pos[5][2],  1.0f, 1.0f, // 17
			 pos[6][0],  pos[6][1], pos[6][2],  1.0f, 0.0f, // 18
			 pos[2][0],  pos[2][1], pos[2][2],  0.0f, 0.0f, // 19

			 // Left
			 pos[4][0],  pos[4][1], pos[4][2],  0.0f, 1.0f, // 20
			 pos[0][0],  pos[0][1], pos[0][2],  1.0f, 1.0f, // 21
			 pos[3][0],  pos[3][1], pos[3][2],  1.0f, 0.0f, // 22
			 pos[7][0],  pos[7][1], pos[7][2],  0.0f, 0.0f, // 23
		};

		unsigned int indices[] = {
			0, 1, 2, 2, 3, 0, // Front
			4, 5, 6, 6, 7, 4, // Back
			8, 9, 10, 10, 11, 8, // Top
			12, 13, 14, 14, 15, 12, // Bottom
			16, 17, 18, 18, 19, 16, // Right
			20, 21, 22, 22, 23, 20  // Left
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glEnable(GL_DEPTH_TEST));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		m_VAO = std::make_unique<VertexArray>();
		m_VBO = std::make_unique<VertexBuffer>(vertices, 24 * 5 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(2);
		m_VAO->AddBuffer(*m_VBO, layout);
		m_IBO = std::make_unique<IndexBuffer>(indices, 36);	

		m_Shader = std::make_unique<Shader>("res/shaders/CubeShader.shader");
		m_Shader->Bind();
		m_Shader->SetUniformMat4("u_Model", m_Model);
		m_Shader->SetUniformMat4("u_View", m_View);
		m_Shader->SetUniformMat4("u_Projection", m_Proj);

		m_Texture = std::make_unique<Texture>("res/textures/doge2.png");
		m_Texture->Bind();
		m_Shader->SetUniform1i("u_Texture", 0);
	}

	RotateCubeTest::~RotateCubeTest()
	{
	}

	void RotateCubeTest::OnUpdate(float deltaTime)
	{
		m_Camera->Update(deltaTime);
	}

	void RotateCubeTest::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		m_View = m_Camera->GetViewMatrix();
		m_Shader->SetUniformMat4("u_View", m_View);

		Renderer renderer;

		// Drawing ten cubes
		for (unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_CubePositions[i]);
			float angle = 20.0f * i * (float)glfwGetTime();
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			m_Shader->SetUniformMat4("u_Model", model);
			renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
		}
	}

	void RotateCubeTest::OnImGuiRender()
	{
		ImGui::Text("Press ESC to show cursor again.");
	}
}
