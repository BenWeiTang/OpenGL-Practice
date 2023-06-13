#include "BoidTest.h"
#include "Renderer.h"
#include <iostream>

namespace test
{
	BoidTest::BoidTest(unsigned int boidCount)
		: m_BoidCount(boidCount)
		, m_ComputeShader(std::make_unique<ComputeShader>("res/shaders/BoidcomputeShader.shader"))
	{
		// Copying positon data to GL
		float* pos = new float[m_BoidCount * 3]{};
		GLCall(glGenBuffers(1, &m_PositionSSBO));
		GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_PositionSSBO));
		GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, m_BoidCount * 3 * sizeof(float), pos, GL_DYNAMIC_READ));
		GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_PositionSSBO));
		GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
		delete[] pos;

		// Test dispatching compute shader
		m_ComputeShader->Bind();
		GLCall(glDispatchCompute(m_BoidCount,1,1));
		GLCall(glMemoryBarrier(GL_ALL_BARRIER_BITS));

		// Test retrieving data
		float secondY = -1.0f;
		GLCall(glGetNamedBufferSubData(m_PositionSSBO, 4 * sizeof(float), sizeof(float), &secondY));
		std::cout << secondY << std::endl;
	}

	BoidTest::~BoidTest()
	{
	}

	void BoidTest::OnUpdate(float deltaTime)
	{
	}

	void BoidTest::OnRender()
	{
	}

	void BoidTest::OnImGuiRender()
	{
	}
}