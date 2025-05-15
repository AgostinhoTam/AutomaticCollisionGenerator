
#include "common.hlsl"


void main(in SKINNING_VS_IN In, out PS_IN Out)
{
	//	重み合成
    matrix skinMatrix =	mul(boneMatrices[In.BoneIndex.x] , In.BoneWeight.x) + 
							mul(boneMatrices[In.BoneIndex.y] , In.BoneWeight.y) +
							mul(boneMatrices[In.BoneIndex.z] , In.BoneWeight.z) +
							mul(boneMatrices[In.BoneIndex.w], In.BoneWeight.w);

	//	頂点ローカル座標スキニング
    float4 posLocal = float4(In.Position.xyz, 1.0f);
    float4 posSkinned = mul(posLocal, skinMatrix);
	
	//	ワイルド行列 x ビュー　x 射影
	matrix wvp;
	wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    Out.Position = mul(posSkinned, wvp);

	Out.TexCoord = In.TexCoord;
	Out.Diffuse = In.Diffuse * Material.Diffuse;

}

