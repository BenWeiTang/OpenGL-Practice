#include "BoidTest.h"
#include "Renderer.h"
#include <iostream>

namespace test
{
	BoidTest::BoidTest(unsigned int boidCount)
		: m_BoidCount(boidCount)
		, m_ComputeShader(std::make_unique<ComputeShader>("res/shaders/BoidcomputeShader.shader"))
	{
		float* pos = new float[m_BoidCount * 4]{};
		float* vel = new float[m_BoidCount * 4]{};
		float* acc = new float[m_BoidCount * 4]{};

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

		// Test dispatching compute shader
		m_ComputeShader->Bind();
		GLCall(glDispatchCompute(m_BoidCount,1,1));
		GLCall(glMemoryBarrier(GL_ALL_BARRIER_BITS));

		// Test retrieving data
		float secondX = -1.0f;
		GLCall(glGetNamedBufferSubData(m_PositionSSBO, 4 * sizeof(float), sizeof(float), &secondX)); // Alignment is actually padded to be like vec4
	}

	BoidTest::~BoidTest()
	{
	}

	void BoidTest::OnUpdate(float deltaTime)
	{
		//m_ComputeShader->SetUniform1f("u_DeltaTime", deltaTime);
		//GLCall(glDispatchCompute(m_BoidCount,1,1));
		//GLCall(glMemoryBarrier(GL_ALL_BARRIER_BITS));
	}

	void BoidTest::OnRender()
	{
	}

	void BoidTest::OnImGuiRender()
	{
	}
}