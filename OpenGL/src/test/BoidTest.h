#pragma once
#include "Test.h"
#include "ComputeShader.h"
#include <memory>

namespace test
{
	class BoidTest : public Test
	{
	public:
		BoidTest(unsigned int boidCount = 1024);
		virtual ~BoidTest();

		virtual void OnUpdate(float deltaTime) override;
		virtual void OnRender() override;
		virtual void OnImGuiRender() override;

	private:
		const unsigned int m_BoidCount;
		std::unique_ptr<ComputeShader> m_ComputeShader;
		unsigned int m_PositionSSBO;
	};
}
