#include "Renderer/renderer.h"
#include "GameObject/gameobject.h"
#include "Manager/modelRendererManager.h"
#include "Renderer/modelRenderer.h"


void ModelRenderer::Draw(const MODEL* Model,const GameObject* Object)
{
	//	���[���h�}�g���N�X�ݒ�
	XMMATRIX world, scale, rot, trans;
	const XMFLOAT3& objPosition = Object->GetPosition();
	const XMFLOAT3& objScale = Object->GetScale();
	const XMFLOAT4& objQuaternion = Object->GetQuaternion();
	const Shader* Shader = Object->GetShader();
	scale = XMMatrixScaling(objScale.x, objScale.y, objScale.z);

	XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(objQuaternion.x, objQuaternion.y, objQuaternion.z);
	quaternion = XMQuaternionNormalize(quaternion);
	rot = XMMatrixRotationQuaternion(quaternion);

	trans = XMMatrixTranslation(objPosition.x, objPosition.y, objPosition.z);
	world = scale * rot * trans;
	Renderer::SetWorldMatrix(world);
	
	//���̓��C�A�E�g�ݒ�
	Renderer::GetDeviceContext()->IASetInputLayout(Shader->m_VertexLayout);

	//�V�F�[�_�[�ݒ�
	Renderer::GetDeviceContext()->VSSetShader(Shader->m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(Shader->m_PixelShader, NULL, 0);


	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &Model->VertexBuffer, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�ݒ�
	Renderer::GetDeviceContext()->IASetIndexBuffer(Model->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// �v���~�e�B�u�g�|���W�ݒ�
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	for( unsigned int i = 0; i < Model->SubsetNum; i++ )
	{
		// �}�e���A���ݒ�
		Renderer::SetMaterial(Model->SubsetArray[i].Material.Material );

		// �e�N�X�`���ݒ�
		if(Model->SubsetArray[i].Material.Texture)
		{
		 	Renderer::GetDeviceContext()->PSSetShaderResources( 0, 1, &Model->SubsetArray[i].Material.Texture );
		}

		// �|���S���`��
		Renderer::GetDeviceContext()->DrawIndexed(Model->SubsetArray[i].IndexNum, Model->SubsetArray[i].StartIndex, 0 );
	}

}



