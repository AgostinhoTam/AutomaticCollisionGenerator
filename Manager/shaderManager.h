#pragma once
#include <unordered_map>
#include "Structure/shaderStructure.h"
#include "Enum/shaderEnum.h"

class ShaderManager
{
public:
	ShaderManager(){}
	~ShaderManager(){}
	static void Init();
	static void Uninit();
	static Shader* CreateShader(const SHADER_NAME& ShaderName,const char* PSFileName,const char* VSFileName);
	static Shader* LoadShader(const SHADER_NAME& ShaderName);
private:
	static std::unordered_map<SHADER_NAME, Shader*> m_ShaderList;
};
