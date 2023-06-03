#pragma once

#include "Test.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Camera.h"
#include <memory>

namespace test
{
	class LightingTest : public Test
	{
	public:
		LightingTest();
		virtual ~LightingTest();

		void OnUpdate(float deltatTime) override;
		void OnRender() override;
		void OnImGuiRender() override;

	private:
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VBO;
		std::unique_ptr<IndexBuffer> m_IBO;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Camera> m_Camera;

		glm::mat4 m_Model;
		glm::mat4 m_View;
		glm::mat4 m_Proj;
	};
}
