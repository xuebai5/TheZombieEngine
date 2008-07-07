//------------------------------------------------------------------------------
//  2.0/zombie/default.fx
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;

matrix<float,4,3> JointPalette[72];

struct VsInput
{
    float4 position : POSITION;
    float4 weights  : BLENDWEIGHT;
    float4 indices  : BLENDINDICES;
};

struct VsOutput
{
    float4 position : POSITION;
};

VsOutput vsMain(const VsInput vsIn, const uniform int geomType)
{
    VsOutput vsOut;
    float4 position = vsIn.position;
    if (geomType == GEOMETRY_SKINNED)
    {
        position = skinnedPosition(position, vsIn.weights, vsIn.indices, JointPalette);
    }
    vsOut.position = mul(position, ModelViewProjection);
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    return float4( 1.f, 0.f, 0.f, 1.f );
}
                           
technique tDefaultStatic
{
    pass p0
    {
        VertexShader    = compile vs_1_1 vsMain(GEOMETRY_DEFAULT);
        PixelShader     = compile ps_1_1 psMain();
    }
}

technique tDefaultSkinned
{
    pass p0
    {
        VertexShader    = compile vs_1_1 vsMain(GEOMETRY_SKINNED);
        PixelShader     = compile ps_1_1 psMain();
    }
}
