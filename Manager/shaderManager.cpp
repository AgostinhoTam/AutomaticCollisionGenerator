#include "System/Renderer/renderer.h"
#include "Manager/shaderManager.h"

std::unordered_map<Shader_Type, Shader*> ShaderManager::m_ShaderList;

void ShaderManager::Init()
{
	CreateShader(Shader_Type::Unlit_Texture, "shader\\unlitTexturePS.cso", "shader\\unlitTextureVS.cso");
	CreateShader(Shader_Type::Debug_Line, "shader\\debugLinePS.cso", "shader\\debugLineVS.cso");
	CreateShader(Shader_Type::Unlit_Skinning_Texture, "shader\\unlitTextureSkinningPS.cso", "shader\\unlitTextureSkinningVS.cso");
}

void ShaderManager::Uninit()
{

	for (auto& pair : m_ShaderList)
	{
		pair.second->m_PixelShader->Release();
		pair.second->m_VertexLayout->Release();
		pair.second->m_VertexShader->Release();
		delete pair.second; // 動的に確保したメモリを解放
	}
	m_ShaderList.clear();
}

Shader* ShaderManager::CreateShader(const Shader_Type& ShaderName,const char* PSFileName, const char* VSFileName)
{
	//	重複できないように
	auto it = m_ShaderList.find(ShaderName);
	if (it != m_ShaderList.end())
	{
		return it->second;
	}

	Shader* shader = new Shader;

	if (ShaderName == Shader_Type::Debug_Line)
	{
		Renderer::CreateDebugVertexShader(&shader->m_VertexShader, &shader->m_VertexLayout, VSFileName);
		Renderer::CreatePixelShader(&shader->m_PixelShader,PSFileName);		// PS現在は１種類だけなので全部同じ
	}
	else if (ShaderName == Shader_Type::Unlit_Skinning_Texture)
	{
		Renderer::CreateSkinningVertexShader(&shader->m_VertexShader, &shader->m_VertexLayout, VSFileName);
		Renderer::CreatePixelShader(&shader->m_PixelShader,PSFileName);
	}
	else
	{
		Renderer::CreateVertexShader(&shader->m_VertexShader, &shader->m_VertexLayout, VSFileName);
		Renderer::CreatePixelShader(&shader->m_PixelShader,PSFileName);
	}


	m_ShaderList.try_emplace(ShaderName, std::move(shader));

	return m_ShaderList[ShaderName];
}

Shader* ShaderManager::LoadShader(const Shader_Type& ShaderName)
{
	auto it = m_ShaderList.find(ShaderName);
	if (it != m_ShaderList.end())
	{
		return it->second;
	}

	return m_ShaderList[Shader_Type::Unlit_Texture];
}
