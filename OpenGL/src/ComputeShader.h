#pragma once
#include <string>

class ComputeShader
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
public:
	ComputeShader(const std::string& filePath);
	~ComputeShader();

	void Bind() const;
	void Unbinde() const;
private:
	std::string ParseComputeShader(const std::string& filePath);
	unsigned int CompileComputeShader(const std::string& source);
	unsigned int CreateComputeShader(const std::string& shader);
};
