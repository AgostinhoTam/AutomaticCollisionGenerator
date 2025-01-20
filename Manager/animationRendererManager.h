#pragma once
#include "System\Enum\modelEnum.h"
#include "System\Renderer/renderer.h"
// マテリアル構造体
//
//struct MODEL_MATERIAL
//{
//	char						Name[256];
//	MATERIAL					Material;
//	char						TextureName[256];
//	ID3D11ShaderResourceView* Texture;
//
//};
//
//
//// 描画サブセット構造体
//struct SUBSET
//{
//	unsigned int	StartIndex;
//	unsigned int	IndexNum;
//	MODEL_MATERIAL	Material;
//};
//
//
//// モデル構造体
//struct MODEL_OBJ
//{
//	VERTEX_3D* VertexArray;
//	unsigned int	VertexNum;
//
//	unsigned int* IndexArray;
//	unsigned int	IndexNum;
//
//	SUBSET* SubsetArray;
//	unsigned int	SubsetNum;
//};
//
//struct MODEL
//{
//	ID3D11Buffer* VertexBuffer;
//	ID3D11Buffer* IndexBuffer;
//
//	SUBSET* SubsetArray;
//	unsigned int	SubsetNum;
//};




#include <string>
#include <unordered_map>

class AnimationModel;
class AnimationRendererManager
{
private:

	static std::unordered_map<MODEL_NAME, AnimationModel*> m_AnimationModelPool;


public:

	static void UnloadAll();

	static AnimationModel* LoadAnimationModel(const MODEL_NAME& Model);

};