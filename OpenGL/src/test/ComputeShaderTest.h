#pragma once
#include "Test.h"
#include "Texture.h"
#include "ComputeShader.h"
#include <memory>

namespace test
{
	class ComputeShaderTest : public Test
	{
	public:
		ComputeShaderTest();
		virtual ~ComputeShaderTest();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		
	private:
		const unsigned int s_TextureWidth;
		const unsigned int s_TextureHeight;

		unsigned int m_Texture;
		std::unique_ptr<ComputeShader> m_ComputeShader;
	};
}