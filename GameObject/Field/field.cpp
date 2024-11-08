
#include "field.h"


Field::Field(XMFLOAT3 location, XMFLOAT2 size):_position(location),GameObject(GAMEOBJECT_TYPE::FIELD)
{

	vertex[0].Position = XMFLOAT3(-50.0f, 0.0f, 50.0f);
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);

	vertex[1].Position = XMFLOAT3(50.0f, 0.0f, 50.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);

	vertex[2].Position = XMFLOAT3(-50.0f, 0.0f, -50.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);

	vertex[3].Position = XMFLOAT3(50.0f, 0.0f, -50.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	Init();
}

void Field::Init()
{
	
	_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
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
	HRESULT hr =LoadFromWICFile(L"asset\\texture\\pattern_shibafu.png", WIC_FLAGS_NONE, &metadata, image);
	if (SUCCEEDED(hr)) {
		CreateShaderResourceView(Renderer::GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_Texture);
		assert(m_Texture);
	}

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");

}

void Field::Uninit()
{
	if(m_VertexBuffer)m_VertexBuffer->Release();
	if(m_Texture)m_Texture->Release();
	
	if(m_VertexLayout)m_VertexLayout->Release();
	if(m_VertexShader)m_VertexShader->Release();
	if(m_PixelShader)m_PixelShader->Release();
}

void Field::Update()
{

	
}

void Field::Draw()
{
	//���̓��C�A�E�g�ݒ�
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//�V�F�[�_�[�ݒ�
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//�}�g���N�X�ݒ�
	XMMATRIX world, scale, rot, trans;
	scale = XMMatrixScaling(_scale.x, _scale.y, _scale.z);
	rot = XMMatrixRotationRollPitchYaw(_rotation.x, _rotation.y, _rotation.z);
	trans = XMMatrixTranslation(_position.x, _position.y, _position.z);
	world = scale * rot * trans;
	Renderer::SetWorldMatrix(world);

	//���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
	
	//�}�e���A��
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.TextureEnable = true;
	Renderer::SetMaterial(material);

	//�e�L�X�`���[�ݒ�
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

	//�v���~�e�B�u�g�|���W�ݒ�
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//�|���R���`��
	Renderer::GetDeviceContext()->Draw(4, 0);

}
