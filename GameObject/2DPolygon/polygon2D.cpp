
#include "Manager/shaderManager.h"
#include "GameObject/2DPolygon/polygon2D.h"


Polygon2D::Polygon2D(XMFLOAT2 location, XMFLOAT2 size)
{
	vertex[0].Position = XMFLOAT3(location.x, location.y + 0.0f, 0.0f);
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);

	vertex[1].Position = XMFLOAT3(location.x + size.x, location.y + 0.0f, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[1].Diffuse = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);

	vertex[2].Position = XMFLOAT3(location.x, location.y + size.y, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[2].Diffuse = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);

	vertex[3].Position = XMFLOAT3(location.x + size.x, location.y + size.y, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	Init();
}

void Polygon2D::Init()
{
	

	//���_�o�b�t�@
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertex;

	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

	//�e�L�X�`���[�ǂݍ���
	TexMetadata metadata;
	ScratchImage image;
	HRESULT hr =LoadFromWICFile(L"asset\\texture\\Brick.png", WIC_FLAGS_NONE, &metadata, image);
	if (SUCCEEDED(hr)) {
		CreateShaderResourceView(Renderer::GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_Texture);
		assert(m_Texture);
	}

	ShaderManager::LoadShader(SHADER_NAME::UNLIT_TEXTURE);

}

void Polygon2D::Uninit()
{
	if(m_VertexBuffer)m_VertexBuffer->Release();
	if(m_Texture)m_Texture->Release();

}

void Polygon2D::Update(const float& DeltaTime)
{

	
}

void Polygon2D::Draw()
{
	//���̓��C�A�E�g�ݒ�
	Renderer::GetDeviceContext()->IASetInputLayout(m_Shader->m_VertexLayout);

	//�V�F�[�_�[�ݒ�
	Renderer::GetDeviceContext()->VSSetShader(m_Shader->m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_Shader->m_PixelShader, NULL, 0);

	//�}�g���N�X�ݒ�		//2D�p�̃}�g���N�X�ɐݒ�
	Renderer::SetWorldViewProjection2D();

	//���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
	
	//�e�L�X�`���[�ݒ�
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

	//�v���~�e�B�u�g�|���W�ݒ�
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//�|���R���`��
	Renderer::GetDeviceContext()->Draw(4, 0);

}
