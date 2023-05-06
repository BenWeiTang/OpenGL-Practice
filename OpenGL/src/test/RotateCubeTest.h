#pragma once

#include "Test.h"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

#include <memory>

namespace test
{
	class RotateCubeTest : public Test
	{
	public:
		RotateCubeTest();
		~RotateCubeTest();

		void OnUpdate(float detaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;

	private:
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VBO;
		std::unique_ptr<IndexBuffer> m_IBO;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture;

		glm::mat4 m_Model;
		glm::mat4 m_View;
		glm::mat4 m_Proj;
		glm::vec3 m_CubePositions[10];
	};
}
