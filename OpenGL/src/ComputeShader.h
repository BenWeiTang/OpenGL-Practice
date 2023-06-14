#pragma once
#include <string>
#include <unordered_map>

class ComputeShader
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	mutable std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	ComputeShader(const std::string& filePath);
	~ComputeShader();

	void Bind() const;
	void Unbinde() const;

	void SetUniform1f(const std::string& name, float value);
private:
	std::string ParseComputeShader(const std::string& filePath);
	unsigned int CompileComputeShader(const std::string& source);
	unsigned int CreateComputeShader(const std::string& shader);
	int GetUniformLocation(const std::string& name) const;
};
