#include "ComputeShader.h"
#include "Renderer.h"
#include <iostream>
#include <fstream>
#include <sstream>

ComputeShader::ComputeShader(const std::string& filePath)
	: m_FilePath(filePath), m_RendererID(0)
{
	std::string source = ParseComputeShader(filePath);
	m_RendererID = CreateComputeShader(source);
}

ComputeShader::~ComputeShader()
{
	GLCall(glDeleteProgram(m_RendererID));
}

void ComputeShader::Bind() const
{
    GLCall(glUseProgram(m_RendererID));
}

void ComputeShader::Unbinde() const
{
    GLCall(glUseProgram(0));
}

void ComputeShader::SetUniform1f(const std::string& name, float value)
{
	GLint location = GetUniformLocation(name);
	GLCall(glUniform1f(location, value));
}

std::string ComputeShader::ParseComputeShader(const std::string& filePath)
{
	std::ifstream stream(m_FilePath);
	std::string line;
	std::stringstream ss;
	while (getline(stream, line))
	{
		ss << line << '\n';
	}
	return ss.str();
}

unsigned int ComputeShader::CompileComputeShader(const std::string& source)
{
	GLCall(unsigned int id = glCreateShader(GL_COMPUTE_SHADER));
	const char* src = source.c_str(); // OpenGL takes a raw string
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));

	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE)
	{
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)alloca(length * sizeof(char));
		GLCall(glGetShaderInfoLog(id, length, &length, message));
		std::cout << "Failed to compile compute shader!" << std::endl;
		std::cout << message << std::endl;
		GLCall(glDeleteShader(id));
		return 0;
	}
	return id;
}


unsigned int ComputeShader::CreateComputeShader(const std::string& shader)
{
    GLCall(unsigned int program = glCreateProgram());
    GLCall(unsigned int cs = CompileComputeShader(shader));

    GLCall(glAttachShader(program, cs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(cs));

    return program;
}

GLint ComputeShader::GetUniformLocation(const std::string& name) const
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	m_UniformLocationCache[name] = location;
	return location;
}
