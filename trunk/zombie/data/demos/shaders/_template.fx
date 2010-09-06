#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;

float4 MatDiffuse = float4( 1.f, 0.f, 0.f, 1.f );

int FillMode = 3; //1=Point, 2=Wireframe, 3=Solid

struct VsInput
{
    float4 position : POSITION;
};

struct VsOutput
{
    float4 position : POSITION;
};

VsOutput vsMain(const VsInput vsIn)
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
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
        ZWriteEnable    = True;
        ZEnable         = True;
        ZFunc           = LessEqual;
        
        FillMode        = <FillMode>;
        CullMode        = None;

        VertexShader    = compile vs_1_1 vsMain();
        PixelShader     = compile ps_1_1 psMain();
    }
}
