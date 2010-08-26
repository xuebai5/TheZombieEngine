#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;

float4 MatDiffuse = float4( 1.f, 0.f, 0.f, 1.f );

struct VsInput
{
    float4 position : POSITION;
};

struct VsOutput
{
    float4 position : POSITION;
};

VsOutput vsMain(const VsInput vsIn, const uniform int geomType)
{
    VsOutput vsOut;
    float4 position = vsIn.position;
    vsOut.position = mul(position, ModelViewProjection);
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    return MatDiffuse;
}
                           
technique t0
{
    pass p0
    {
        VertexShader    = compile vs_1_1 vsMain(GEOMETRY_DEFAULT);
        PixelShader     = compile ps_1_1 psMain();
    }
}
