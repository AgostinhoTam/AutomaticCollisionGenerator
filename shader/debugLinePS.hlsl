#include "common.hlsl"

void main(in DEBUG_PS_IN In, out float4 outDiffuse : SV_Target)
{
    outDiffuse = In.Diffuse;
}
