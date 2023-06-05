#pragma once
#include "Test.h"
#include "Texture.h"
#include "ComputeShader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
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
		std::unique_ptr<Shader> m_ScreenQaudShader;
		std::unique_ptr<ComputeShader> m_ComputeShader;
		std::unique_ptr<VertexArray> m_QuadVAO;
		std::unique_ptr<VertexBuffer> m_QuadVBO;
		std::unique_ptr<IndexBuffer> m_QuadIBO;
	};
}