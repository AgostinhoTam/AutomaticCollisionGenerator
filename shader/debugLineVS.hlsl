#include "common.hlsl"


void main(in DEBUG_VS_IN In, out DEBUG_PS_IN Out)
{

    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);

    Out.Position = mul(In.Position, wvp);
    Out.Diffuse = In.Diffuse;

}

