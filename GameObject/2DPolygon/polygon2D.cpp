/*===================================================================================

	2D板ポリゴン_UI用(polygon2D.cpp)

====================================================================================*/
#include "Manager/shaderManager.h"
#include "GameObject/2DPolygon/polygon2D.h"

//	=========================２D板ポリゴン=========================
//	Location	:	XMFLOAT2	位置（左上基準）
//	Size	:	XMFLOAT2	大きさ
Polygon2D::Polygon2D(XMFLOAT2 Location, XMFLOAT2 Size)
{
	vertex[0].Position = XMFLOAT3(Location.x, Location.y + 0.0f, 0.0f);
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);

	vertex[1].Position = XMFLOAT3(Location.x + Size.x, Location.y + 0.0f, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[1].Diffuse = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);

	vertex[2].Position = XMFLOAT3(Location.x, Location.y + Size.y, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[2].Diffuse = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);

	vertex[3].Position = XMFLOAT3(Location.x + Size.x, Location.y + Size.y, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	Init();
}
//	=========================２D板ポリゴン初期化=========================
void Polygon2D::Init()
{
	//頂点バッファ
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertex;

	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

	//テキスチャー読み込み
	TexMetadata metadata;
	ScratchImage image;
	HRESULT hr =LoadFromWICFile(L"asset\\texture\\Brick.png", WIC_FLAGS_NONE, &metadata, image);
	if (SUCCEEDED(hr)) {
		CreateShaderResourceView(Renderer::GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_Texture);
		assert(m_Texture);
	}

	ShaderManager::LoadShader(Shader_Type::Unlit_Texture);
}

//	=========================２D板ポリゴンUninit=========================
void Polygon2D::Uninit()
{
	if(m_VertexBuffer)m_VertexBuffer->Release();
	if(m_Texture)m_Texture->Release();

}

//	=========================２D板ポリゴン更新=========================
//	DeltaTime	:	float	デルタタイム
void Polygon2D::Update(const float& DeltaTime)
{
	
}

//	=========================２D板ポリゴン描画=========================
void Polygon2D::Draw()
{
	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_Shader->m_VertexLayout);

	//シェーダー設定
	Renderer::GetDeviceContext()->VSSetShader(m_Shader->m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_Shader->m_PixelShader, NULL, 0);

	//マトリクス設定		//2D用のマトリクスに設定
	Renderer::SetWorldViewProjection2D();

	//頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
	
	//テキスチャー設定
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

	//プリミティブトポロジ設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//ポリコン描画
	Renderer::GetDeviceContext()->Draw(4, 0);

}
