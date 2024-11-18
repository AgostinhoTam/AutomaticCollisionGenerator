#include "Renderer/renderer.h"
#include "Manager/modelRendererManager.h"
#include "Renderer/modelRenderer.h"


void ModelRenderer::Draw(const MODEL*& model)
{

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &model->VertexBuffer, &stride, &offset);

	// インデックスバッファ設定
	Renderer::GetDeviceContext()->IASetIndexBuffer(model->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// プリミティブトポロジ設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	for( unsigned int i = 0; i < model->SubsetNum; i++ )
	{
		// マテリアル設定
		Renderer::SetMaterial(model->SubsetArray[i].Material.Material );

		// テクスチャ設定
		if(model->SubsetArray[i].Material.Texture)
			Renderer::GetDeviceContext()->PSSetShaderResources( 0, 1, &model->SubsetArray[i].Material.Texture );

		// ポリゴン描画
		Renderer::GetDeviceContext()->DrawIndexed(model->SubsetArray[i].IndexNum, model->SubsetArray[i].StartIndex, 0 );
	}

}



