#include "BoidTest.h"
#include "GL/glew.h"
#include "imgui/imgui.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <cstdlib>

namespace test
{
	BoidTest::BoidTest(unsigned int boidCount)
		: m_BoidCount(boidCount)
		, m_ComputeShader(std::make_unique<ComputeShader>("res/shaders/BoidcomputeShader.shader"))
		, m_BoidShader(std::make_unique<Shader>("res/shaders/BoidShader.shader"))
		, m_SeperationFactor(1.0f), m_AlignmentFactor(1.0f), m_CohesionFactor(1.0f)
	{
		float* pos = new float[m_BoidCount * 4]{};
		float* vel = new float[m_BoidCount * 4]{};
		float* acc = new float[m_BoidCount * 4]{};

		// Set random starting values for pos, vel, and acc
		for (int i = 0; i < m_BoidCount; i++)
		{
			pos[i * 4 + 0] = (std::rand() % 10000 - 5000) / 100.0f;
			pos[i * 4 + 1] = (std::rand() % 10000 - 5000) / 100.0f;
			pos[i * 4 + 2] = (std::rand() % 10000 - 5000) / 100.0f;
			pos[i * 4 + 3] = 1.0f;
			vel[i * 4 + 0] = (std::rand() % 10000 - 5000) / 100.0f;
			vel[i * 4 + 1] = (std::rand() % 10000 - 5000) / 100.0f;
			vel[i * 4 + 2] = (std::rand() % 10000 - 5000) / 100.0f;
			vel[i * 4 + 3] = 0.0f;
			acc[i * 4 + 0] = (std::rand() % 2000 - 1000) / 100.0f;
			acc[i * 4 + 1] = (std::rand() % 2000 - 1000) / 100.0f;
			acc[i * 4 + 2] = (std::rand() % 2000 - 1000) / 100.0f;
			acc[i * 4 + 3] = 0.0f;
		}

		// Generate buffers on CPU
		GLCall(glGenBuffers(1, &m_PositionSSBO));
		GLCall(glGenBuffers(1, &m_VelocitySSBO));
		GLCall(glGenBuffers(1, &m_AccelerationSSBO));
		GLCall(glGenBuffers(1, &m_TransMatrixSSBO));

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

		// Generate an uninitialized SSBO for the transformation matrices and bind it to index 3
		GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_TransMatrixSSBO));
		GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, m_BoidCount * 16 * sizeof(float), nullptr, GL_DYNAMIC_READ));
		GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_TransMatrixSSBO));

		GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0)); // Unbind

		// Clean up
		delete[] pos;
		delete[] vel;
		delete[] acc;

		GLCall(glEnable(GL_BLEND));
		GLCall(glEnable(GL_DEPTH_TEST));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		
		// Pyramid (homogeneous coord as 1)
		float vertices[] = {
			-0.1f, -0.1f, -0.1f, 1.0f, // 0 Front Left
			 0.1f, -0.1f, -0.1f, 1.0f, // 1 Front Right 
			 0.1f, -0.1f,  0.1f, 1.0f, // 2 Back Right
			-0.1f, -0.1f,  0.1f, 1.0f, // 3 Back Left
			 0.0f,  0.1f,  0.0f, 1.0f  // 4 Top
		};

		// Generate and bind to VAO
		GLCall(glGenVertexArrays(1, &m_VAO));
		GLCall(glBindVertexArray(m_VAO));

		// Generate and pass vertex data into VBO
		GLCall(glGenBuffers(1, &m_VBO));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
		// For every vertex, there are 4 floats for the positions and 1 uint for the index
		// For every boid (pyramid), there are 5 vertices
		GLCall(glBufferData(GL_ARRAY_BUFFER, 5 * m_BoidCount * (4 * sizeof(float) + sizeof(unsigned int)), nullptr, GL_STATIC_DRAW));
		// Vertex positions
		// Pack all positional data in the beginning of the VBO
		for (int i = 0; i < m_BoidCount; i++)
		{
			GLCall(glBufferSubData(GL_ARRAY_BUFFER, i * sizeof(vertices), sizeof(vertices), vertices));
		}
		// Vertext id
		// Pack all index data tightly together after the positional data
		for (unsigned int i = 0; i < m_BoidCount; i++)
		{
			unsigned int index[5] = { i, i, i, i, i };
			GLCall(glBufferSubData(GL_ARRAY_BUFFER, i * 5 * sizeof(unsigned int) + m_BoidCount * sizeof(vertices), 5 * sizeof(unsigned int), index));
		}
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void*)nullptr));
		GLCall(glEnableVertexAttribArray(1));
		GLCall(glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(unsigned int), (const void*)(m_BoidCount * sizeof(vertices))));

		// Generate and pass index data into IBO
		const unsigned int indexCount = m_BoidCount * 18;
		unsigned int* indices = new unsigned int[indexCount];
		unsigned int offset = 0;
		for (int i = 0; i < indexCount; i+=18)
		{
			indices[i +  0] = 0 + offset; indices[i +  1] = 1 + offset; indices[i +  2] = 2 + offset; // Bottom 1
			indices[i +  3] = 2 + offset; indices[i +  4] = 3 + offset; indices[i +  5] = 0 + offset; // Bottom 2
			indices[i +  6] = 0 + offset; indices[i +  7] = 1 + offset; indices[i +  8] = 4 + offset; // Front
			indices[i +  9] = 1 + offset; indices[i + 10] = 2 + offset; indices[i + 11] = 4 + offset; // Right
			indices[i + 12] = 2 + offset; indices[i + 13] = 3 + offset; indices[i + 14] = 4 + offset; // Back
			indices[i + 15] = 3 + offset; indices[i + 16] = 0 + offset; indices[i + 17] = 4 + offset; // Left
			offset += 5;
		}
		GLCall(glGenBuffers(1, &m_IBO));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO));
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW)); // Indices don't change much, ig, lol
		delete[] indices;

		m_BoidShader->Bind();
		glm::mat4 viewMatrix = glm::lookAt(glm::vec3(100.0f, 100.0f, -100.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		m_BoidShader->SetUniformMat4("u_View", viewMatrix);
		m_BoidShader->SetUniformMat4("u_Projection", glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 300.0f));
	}

	BoidTest::~BoidTest()
	{
	}

	void BoidTest::OnUpdate(float deltaTime)
	{
		m_ComputeShader->Bind();
		m_ComputeShader->SetUniform1f("u_DeltaTime", deltaTime);
		m_ComputeShader->SetUniform1f("u_SeperationFactor", m_SeperationFactor);
		m_ComputeShader->SetUniform1f("u_AlignmentFactor", m_AlignmentFactor);
		m_ComputeShader->SetUniform1f("u_CohesionFactor", m_CohesionFactor);
		GLCall(glDispatchCompute(m_BoidCount / 1024,1,1));
		GLCall(glMemoryBarrier(GL_ALL_BARRIER_BITS));
	}

	void BoidTest::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		m_BoidShader->Bind();
		GLCall(glBindVertexArray(m_VAO));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO));
		GLCall(glDrawElements(GL_TRIANGLES, 18 * m_BoidCount, GL_UNSIGNED_INT, nullptr));
	}

	void BoidTest::OnImGuiRender()
	{
		ImGui::Text("Press ESC to show cursor again.");
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Begin("Controls");
		ImGui::DragFloat("Seperation", &m_SeperationFactor, 0.1f, 0.0f, 10.0f);
		ImGui::DragFloat("Alignment", &m_AlignmentFactor, 0.1f, 0.0f, 10.0f);
		ImGui::DragFloat("Cohesion", &m_CohesionFactor, 0.1f, 0.0f, 10.0f);
		ImGui::End();
	}
}