#pragma once

#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "Renderer.h"

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();

	// For every attribute of every vertex in this vertex buffer...
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		// nth element is more like the nth attribute
		const auto& element = elements[i];

		// Enable this attribute for rendering, specs are below...
		GLCall(glEnableVertexAttribArray(i));

		/*	i is the same with the one just enabled
			element.count must be 1, 2, 3, or 4
			element.type tells how big to multiply by element.count so we know the size of this attribute
			layout.GetStride() is the size of an entire vertex in this vb
			offset is how far off from the beginning of the stride, or essentially the sum of all previous attributes' sizes
		*/
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset));

		// offset is the size of this attribute that we just specified to gl, which is number * type, e.g., vec2f is two floats which is 2 * 4 bytes
		offset += element.count * VertextBufferElement::GetSizeOfType(element.type);
	}
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}
