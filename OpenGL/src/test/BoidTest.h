#pragma once
#include "Test.h"
#include "Shader.h"
#include "ComputeShader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include <memory>

namespace test
{
	class BoidTest : public Test
	{
	public:
		BoidTest(unsigned int boidCount = 16384);
		virtual ~BoidTest();

		virtual void OnUpdate(float deltaTime) override;
		virtual void OnRender() override;
		virtual void OnImGuiRender() override;

	private:
		const unsigned int m_BoidCount;
		std::unique_ptr<ComputeShader> m_ComputeShader;
		unsigned int m_PositionSSBO;
		unsigned int m_VelocitySSBO;
		unsigned int m_AccelerationSSBO;
		unsigned int m_TransMatrixSSBO;
		std::unique_ptr<Shader> m_BoidShader;
		unsigned int m_VAO;
		unsigned int m_VBO;
		unsigned int m_IBO;
		float m_SeperationFactor, m_AlignmentFactor, m_CohesionFactor;
	};
}
