#pragma once
#include <unordered_map>
#include "System\Structure/shaderStructure.h"
#include "System\Enum/shaderEnum.h"

class ShaderManager
{
private:
	static std::unordered_map<Shader_Type, Shader*> m_ShaderList;

public:
	ShaderManager(){}
	~ShaderManager(){}
	static void Init();
	static void Uninit();
	static Shader* CreateShader(const Shader_Type& ShaderName,const char* PSFileName,const char* VSFileName);
	static Shader* LoadShader(const Shader_Type& ShaderName);
};
