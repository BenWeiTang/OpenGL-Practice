#include "ComputeShaderTest.h"
#include "Renderer.h"
#include <iostream>

namespace test
{
	ComputeShaderTest::ComputeShaderTest()
		: s_TextureWidth(512), s_TextureHeight(512), m_ComputeShader(std::make_unique<ComputeShader>("res/shaders/ComputeShader.shader"))
		, m_ScreenQaudShader(std::make_unique<Shader>("res/shaders/ScreenQuad.shader"))
	{
		// Identical to the Texture class but it cannot handle the call to "glBindImageTexture" for its generic purpose
		GLCall(glGenTextures(1, &m_Texture));
		GLCall(glActiveTexture(GL_TEXTURE0));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_Texture));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, s_TextureWidth, s_TextureHeight, 0, GL_RGBA, GL_FLOAT, NULL));
		GLCall(glBindImageTexture(1, m_Texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F)); //For some reason, compute shader cannot be bound to binding=0

		// Bind to shader we want to use to draw on a quad
		m_ScreenQaudShader->Bind();
		m_ScreenQaudShader->SetUniform1i("u_Texture", 0);

		// Making a quad shape
		float vertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		unsigned int indices[] = {
			0, 1, 2,
			1, 2, 3
		};
		m_QuadVAO = std::make_unique<VertexArray>();
		m_QuadVBO = std::make_unique<VertexBuffer>(vertices, 4 * 5 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(2);
		m_QuadVAO->AddBuffer(*m_QuadVBO, layout);
		m_QuadIBO = std::make_unique<IndexBuffer>(indices, 6);
	}

	ComputeShaderTest::~ComputeShaderTest()
	{
	}

	void ComputeShaderTest::OnUpdate(float deltaTime)
	{
	}

	void ComputeShaderTest::OnRender()
	{
		// Bind to compute shader and do calcuations
		m_ComputeShader->Bind();
		GLCall(glDispatchCompute(s_TextureWidth, s_TextureHeight, 1));
		GLCall(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT)); // Can also use GL_ALL_BARRIER_BITS

		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		// Render quad
		Renderer renderer;
		renderer.Draw(*m_QuadVAO, *m_QuadIBO, *m_ScreenQaudShader);
	}

	void ComputeShaderTest::OnImGuiRender()
	{
	}
}