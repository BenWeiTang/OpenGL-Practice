#include "LightingTest.h"

#include "Renderer.h"

#include <iostream>

namespace test
{
	LightingTest::LightingTest()
		: m_Camera(std::make_unique<Camera>()),
		m_Model(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
		m_View(m_Camera->GetViewMatrix()),
		m_Proj(glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 100.0f))
	{
		float vertices[]
		{
			-0.5f,  0.5f,  0.5f, // 0 Front Top Left
			 0.5f,  0.5f,  0.5f, // 1 Front Top Right
			 0.5f, -0.5f,  0.5f, // 2 Front Bottom Right
			-0.5f, -0.5f,  0.5f, // 3 Front Bottom Left
			-0.5f,  0.5f, -0.5f, // 4 Back Top Left
			 0.5f,  0.5f, -0.5f, // 5 Back Top Right
			 0.5f, -0.5f, -0.5f, // 6 Back Bottom Right
			-0.5f, -0.5f, -0.5f, // 7 Back Bottom Left
		};

		unsigned int indices[]
		{
			0, 1, 2, 2, 3, 0, // Front
			4, 5, 6, 6, 7, 4, // Back
			0, 1, 5, 5, 4, 0, // Top
			3, 2, 6, 6, 7, 3, // Bottom
			0, 3, 7, 7, 4, 0, // Left
			1, 2, 6, 6, 5, 1  // Right
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glEnable(GL_DEPTH_TEST));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		m_VAO = std::make_unique<VertexArray>();
		m_VBO = std::make_unique<VertexBuffer>(vertices, 8 * 3 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(3);
		m_VAO->AddBuffer(*m_VBO, layout);
		m_IBO = std::make_unique<IndexBuffer>(indices, 36);	

		m_Shader = std::make_unique<Shader>("res/shaders/LightingCube.shader");
		m_Shader->Bind();
		m_Shader->SetUniformMat4("u_Model", m_Model);
		m_Shader->SetUniformMat4("u_View", m_View);
		m_Shader->SetUniformMat4("u_Projection", m_Proj);
	}

	LightingTest::~LightingTest()
	{
	}

	void LightingTest::OnUpdate(float deltatTime)
	{
		m_Camera->Update(deltatTime);
	}

	void LightingTest::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		m_View = m_Camera->GetViewMatrix();
		m_Shader->SetUniformMat4("u_View", m_View);

		Renderer renderer;
		renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
	}

	void LightingTest::OnImGuiRender()
	{
	}
}