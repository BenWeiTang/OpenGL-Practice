#include "BoidTest.h"
#include "Renderer.h"
#include "imgui/imgui.h"
#include <iostream>
#include <cstdlib>

namespace test
{
	BoidTest::BoidTest(unsigned int boidCount)
		: m_BoidCount(boidCount)
		, m_ComputeShader(std::make_unique<ComputeShader>("res/shaders/BoidcomputeShader.shader"))
		, m_Camera(std::make_unique<Camera>())
		, m_BoidShader(std::make_unique<Shader>("res/shaders/BoidShader.shader"))
	{
		float* pos = new float[m_BoidCount * 4]{};
		float* vel = new float[m_BoidCount * 4]{};
		float* acc = new float[m_BoidCount * 4]{};

		for (int i = 0; i < m_BoidCount; i++)
		{
			pos[i * 4 + 0] = (float)(std::rand() % 50);
			pos[i * 4 + 1] = (float)(std::rand() % 50);
			pos[i * 4 + 2] = (float)(std::rand() % 50);
			pos[i * 4 + 3] = 0.0f;
		}

		// Generate buffers on CPU
		GLCall(glGenBuffers(1, &m_PositionSSBO));
		GLCall(glGenBuffers(1, &m_VelocitySSBO));
		GLCall(glGenBuffers(1, &m_AccelerationSSBO));

		// Copy data into position buffer and bind it to index 0
		GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_PositionSSBO));
		GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, m_BoidCount * 4 * sizeof(float), pos, GL_DYNAMIC_READ));
		GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_PositionSSBO));

		// Copy data into velocity buffer and bind it to index 1
		GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_VelocitySSBO));
		GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, m_BoidCount * 4 * sizeof(float), vel, GL_DYNAMIC_READ));
		GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_VelocitySSBO));

		// Copy data into acceleration buffer and bind it to index 2
		GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_AccelerationSSBO));
		GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, m_BoidCount * 4 * sizeof(float), acc, GL_DYNAMIC_READ));
		GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_AccelerationSSBO));

		GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0)); // Unbind

		// Clean up
		delete[] pos;
		delete[] vel;
		delete[] acc;

		//GLCall(glEnable(GL_BLEND));
		//GLCall(glEnable(GL_DEPTH_TEST));
		//GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		
		// Pyramid
		float vertices[] = {
			-0.1f, -0.1f, -0.1f, // 0 Front Left
			 0.1f, -0.1f, -0.1f, // 1 Front Right 
			 0.1f, -0.1f,  0.1f, // 2 Back Right
			-0.1f, -0.1f,  0.1f, // 3 Back Left
			 0.0f,  0.1f,  0.0f  // 4 Top
		};

		unsigned int indices[] = {
			0, 1, 2, // Bottom 1
			2, 3, 0, // Bottom 2
			0, 1, 4, // Front
			1, 2, 4, // Right
			2, 3, 4, // Back
			3, 0, 4  // Left
		};

		m_VAO = std::make_unique<VertexArray>();
		m_VBO = std::make_unique<VertexBuffer>(vertices, sizeof(vertices));
		VertexBufferLayout layout;
		layout.Push<float>(3);
		m_VAO->AddBuffer(*m_VBO, layout);
		m_IBO = std::make_unique<IndexBuffer>(indices, sizeof(indices)/sizeof(unsigned int));

		m_BoidShader->Bind();
		m_BoidShader->SetUniformMat4("u_Model", glm::mat4(1.0f));
		m_BoidShader->SetUniformMat4("u_Projection", glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 100.0f));

		// Test dispatching compute shader
		//m_ComputeShader->Bind();
		//GLCall(glDispatchCompute(m_BoidCount,1,1));
		//GLCall(glMemoryBarrier(GL_ALL_BARRIER_BITS));

		// Test retrieving data
		//float secondX = -1.0f;
		//GLCall(glGetNamedBufferSubData(m_PositionSSBO, 4 * sizeof(float), sizeof(float), &secondX)); // Alignment is actually padded to be like vec4
	}

	BoidTest::~BoidTest()
	{
	}

	void BoidTest::OnUpdate(float deltaTime)
	{
		m_Camera->Update(deltaTime);
		m_ComputeShader->Bind();
		m_ComputeShader->SetUniform1f("u_DeltaTime", deltaTime);
		GLCall(glDispatchCompute(m_BoidCount,1,1));
		GLCall(glMemoryBarrier(GL_ALL_BARRIER_BITS));
	}

	void BoidTest::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		Renderer renderer;
		m_BoidShader->Bind();

		for (int i = 0; i < m_BoidCount; i++)
		{
			float dis[3] = {};
			GLCall(glGetNamedBufferSubData(m_PositionSSBO, i * 4 * sizeof(float), 3 * sizeof(float), dis));
			m_BoidShader->SetUniformMat4("u_Model", glm::translate(glm::mat4(1.0f), glm::vec3(dis[0], dis[1], dis[2])));
			m_BoidShader->SetUniformMat4("u_View", m_Camera->GetViewMatrix());
			renderer.Draw(*m_VAO, *m_IBO, *m_BoidShader);
		}
	}

	void BoidTest::OnImGuiRender()
	{
		ImGui::Text("Press ESC to show cursor again.");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}