#include "Renderer/renderer.h"
#include "GameObject/gameobject.h"
#include "Manager/modelRendererManager.h"
#include "Renderer/modelRenderer.h"


void ModelRenderer::Draw(const MODEL* Model,const GameObject* Object)
{
	//	ワールドマトリクス設定
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
	
	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(Shader->m_VertexLayout);

	//シェーダー設定
	Renderer::GetDeviceContext()->VSSetShader(Shader->m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(Shader->m_PixelShader, NULL, 0);


	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &Model->VertexBuffer, &stride, &offset);

	// インデックスバッファ設定
	Renderer::GetDeviceContext()->IASetIndexBuffer(Model->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// プリミティブトポロジ設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	for( unsigned int i = 0; i < Model->SubsetNum; i++ )
	{
		// マテリアル設定
		Renderer::SetMaterial(Model->SubsetArray[i].Material.Material );

		// テクスチャ設定
		if(Model->SubsetArray[i].Material.Texture)
		{
		 	Renderer::GetDeviceContext()->PSSetShaderResources( 0, 1, &Model->SubsetArray[i].Material.Texture );
		}

		// ポリゴン描画
		Renderer::GetDeviceContext()->DrawIndexed(Model->SubsetArray[i].IndexNum, Model->SubsetArray[i].StartIndex, 0 );
	}

}



