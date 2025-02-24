
#include "common.hlsl"


void main(in SKINNING_VS_IN In, out PS_IN Out)
{
	//	�d�ݍ���
    matrix skinMatrix =	mul(boneMatrices[In.BoneIndex.x] , In.BoneWeight.x) + 
							mul(boneMatrices[In.BoneIndex.y] , In.BoneWeight.y) +
							mul(boneMatrices[In.BoneIndex.z] , In.BoneWeight.z) +
							mul(boneMatrices[In.BoneIndex.w], In.BoneWeight.w);

	//	���_���[�J�����W�X�L�j���O
    float4 posLocal = float4(In.Position.xyz, 1.0f);
    float4 posSkinned = mul(posLocal, skinMatrix);
	
	//	���C���h�s�� x �r���[�@x �ˉe
	matrix wvp;
	wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    Out.Position = mul(posSkinned, wvp);

	Out.TexCoord = In.TexCoord;
	Out.Diffuse = In.Diffuse * Material.Diffuse;

}

