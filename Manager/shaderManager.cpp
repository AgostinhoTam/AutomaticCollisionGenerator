#include "System\Renderer/renderer.h"
#include "Manager/shaderManager.h"

std::unordered_map<SHADER_NAME, Shader*> ShaderManager::m_ShaderList;

void ShaderManager::Init()
{
	CreateShader(SHADER_NAME::UNLIT_TEXTURE);
	CreateShader(SHADER_NAME::DEBUG_LINE);
	CreateShader(SHADER_NAME::UNLIT_TEXTURE_SKINNING);
}

void ShaderManager::Uninit()
{

	for (auto& pair : m_ShaderList)
	{
		pair.second->m_PixelShader->Release();
		pair.second->m_VertexLayout->Release();
		pair.second->m_VertexShader->Release();
		delete pair.second; // “®“I‚ÉŠm•Û‚µ‚½ƒƒ‚ƒŠ‚ð‰ð•ú
	}
	m_ShaderList.clear();
}

Shader* ShaderManager::CreateShader(const SHADER_NAME& ShaderName)
{
	//	d•¡‚Å‚«‚È‚¢‚æ‚¤‚É
	auto it = m_ShaderList.find(ShaderName);
	if (it != m_ShaderList.end())
	{
		return it->second;
	}

	Shader* shader = new Shader;

	if (ShaderName == SHADER_NAME::DEBUG_LINE)
	{
		Renderer::CreateDebugVertexShader(&shader->m_VertexShader, &shader->m_VertexLayout, "shader\\debugLineVS.cso");
		Renderer::CreatePixelShader(&shader->m_PixelShader, "shader\\debugLinePS.cso");
	}
	else if (ShaderName == SHADER_NAME::UNLIT_TEXTURE_SKINNING)
	{
		Renderer::CreateSkinningVertexShader(&shader->m_VertexShader, &shader->m_VertexLayout, "shader\\unlitTextureSkinningVS.cso");
		Renderer::CreatePixelShader(&shader->m_PixelShader, "shader\\unlitTextureSkinningPS.cso");
	}
	else
	{
		Renderer::CreateVertexShader(&shader->m_VertexShader, &shader->m_VertexLayout, "shader\\unlitTextureVS.cso");
		Renderer::CreatePixelShader(&shader->m_PixelShader, "shader\\unlitTexturePS.cso");
	}


	m_ShaderList.try_emplace(ShaderName, std::move(shader));

	return m_ShaderList[ShaderName];
}

Shader* ShaderManager::LoadShader(const SHADER_NAME& ShaderName)
{
	auto it = m_ShaderList.find(ShaderName);
	if (it != m_ShaderList.end())
	{
		return it->second;
	}

	return m_ShaderList[SHADER_NAME::UNLIT_TEXTURE];
}
