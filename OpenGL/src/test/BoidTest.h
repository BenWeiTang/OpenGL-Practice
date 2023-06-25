#pragma once
#include "Test.h"
#include "Shader.h"
#include "ComputeShader.h"
#include "Camera.h"
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
		BoidTest(unsigned int boidCount = 512);
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
		std::unique_ptr<Camera> m_Camera;
		std::unique_ptr<Shader> m_BoidShader;
		//std::unique_ptr<VertexArray> m_VAO;
		//std::unique_ptr<VertexBuffer> m_VBO;
		//std::unique_ptr<IndexBuffer> m_IBO;
		unsigned int m_VAO;
		unsigned int m_VBO;
		unsigned int m_IBO;
	};
}
