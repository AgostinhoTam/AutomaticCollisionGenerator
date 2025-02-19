#pragma once
#include "Main/main.h"

struct VERTEX_3D
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT4 Diffuse;
	XMFLOAT2 TexCoord;
};

struct VERTEX_3D_SKIN
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;
	XMFLOAT4 Diffuse;
	UINT BoneIndex[4];
	float BoneWeight[4];
};


struct MATERIAL
{
	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT4	Emission;
	float		Shininess;
	BOOL		TextureEnable;
	float		Dummy[2];
};



struct LIGHT
{
	BOOL		Enable;
	BOOL		Dummy[3];
	XMFLOAT4	Direction;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Ambient;
};

enum class CULL_MODE
{
	CULL_MODE_NONE,			//カリング無し
	CULL_MODE_FRONT,		//表のポリゴンを描画しない
	CULL_MODE_BACK,			//裏のポリゴンを描画しない

	CULL_MODE_NUM
};

enum class BLEND_MODE
{
	BLEND_MODE_NONE,		//ブレンド無し
	BLEND_MODE_ALPHABLEND,	//αブレンド
	BLEND_MODE_ADD,			//加算ブレンド
	BLEND_MODE_ATC			//Alpha-To-Coverage、草なとの半透明表現
};
class Renderer
{
private:

	static D3D_FEATURE_LEVEL       m_FeatureLevel;

	static ID3D11Device*           m_Device;
	static ID3D11DeviceContext*    m_DeviceContext;
	static IDXGISwapChain*         m_SwapChain;
	static ID3D11RenderTargetView* m_RenderTargetView;
	static ID3D11DepthStencilView* m_DepthStencilView;

	static ID3D11Buffer*			m_WorldBuffer;
	static ID3D11Buffer*			m_ViewBuffer;
	static ID3D11Buffer*			m_ProjectionBuffer;
	static ID3D11Buffer*			m_MaterialBuffer;
	static ID3D11Buffer*			m_LightBuffer;
	static ID3D11Buffer*			m_ColorBuffer;
	static ID3D11Buffer*			m_BoneMatricesBuffer;

	static ID3D11DepthStencilState* m_DepthStateEnable;
	static ID3D11DepthStencilState* m_DepthStateDisable;

	static ID3D11BlendState*		m_BlendState;
	static ID3D11BlendState*		m_BlendStateATC;
	static ID3D11BlendState*		m_BlendStateAdd;

	static ID3D11RasterizerState*	m_RasterStateCullOff;
	static ID3D11RasterizerState*	m_RasterStateCullFront;
	static ID3D11RasterizerState*	m_RasterStateCullBack;


public:
	static void Init();
	static void Uninit();
	static void Begin();
	static void End();

	static void SetDepthEnable(bool Enable);
	static void SetCullingMode(const CULL_MODE& CullMode);
	static void SetATCEnable(bool Enable);
	static void SetBlendState(const BLEND_MODE& BlendMode);
	static void SetWorldViewProjection2D();
	static void SetWorldMatrix(const XMMATRIX& WorldMatrix);
	static void SetViewMatrix(const XMMATRIX& ViewMatrix);
	static void SetProjectionMatrix(const XMMATRIX& ProjectionMatrix);
	static void SetMaterial(const MATERIAL& Material);
	static void SetLight(const LIGHT& Light);
	static void SetColor(const XMFLOAT4& Color);
	static void SetBoneMatrices(const XMMATRIX& BoneMatrices);

	static ID3D11Device* GetDevice( void ){ return m_Device; }
	static ID3D11DeviceContext* GetDeviceContext( void ){ return m_DeviceContext; }

	static void CreateVertexShader(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName);
	static void CreateDebugVertexShader(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName);
	static void CreateSkinningVertexShader(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName);
	static void CreatePixelShader(ID3D11PixelShader** PixelShader, const char* FileName);


};
