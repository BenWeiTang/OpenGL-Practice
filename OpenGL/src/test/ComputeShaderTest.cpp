#include "ComputeShaderTest.h"
#include "Renderer.h"

namespace test
{
	ComputeShaderTest::ComputeShaderTest()
		: s_TextureWidth(512), s_TextureHeight(512), m_ComputeShader(std::make_unique<ComputeShader>("res/shaders/ComputeShader.shader"))
	{
		GLCall(glGenTextures(1, &m_Texture));
		GLCall(glActiveTexture(GL_TEXTURE0));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_Texture));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, s_TextureWidth, s_TextureHeight, 0, GL_RGBA, GL_FLOAT, NULL));
		GLCall(glBindImageTexture(0, m_Texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F));

		m_ComputeShader->Bind();
	}

	ComputeShaderTest::~ComputeShaderTest()
	{
	}

	void ComputeShaderTest::OnUpdate(float deltaTime)
	{
	}

	void ComputeShaderTest::OnRender()
	{
		GLCall(glDispatchCompute(s_TextureWidth, s_TextureHeight, 1));
		GLCall(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));

		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}

	void ComputeShaderTest::OnImGuiRender()
	{
	}
}