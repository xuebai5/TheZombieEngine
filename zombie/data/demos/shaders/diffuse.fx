//------------------------------------------------------------------------------
//  diffuse.fx
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;

matrix<float,4,3> JointPalette[72];

int mipMapLodBias;

int FillMode = 3; //1=Point, 2=Wireframe, 3=Solid
int CullMode = 2; //1=NoCull, 2=CW, 3=CCW

texture DiffMap0 : DIFFUSE;

sampler2D ColorSampler = sampler_state
{
    Texture = <DiffMap0>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
}; 

struct VsInput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float4 weights  : BLENDWEIGHT;
    float4 indices  : BLENDINDICES;
};

struct VsOutput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
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
    vsOut.uv0 = vsIn.uv0;
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    return tex2D( ColorSampler, psIn.uv0 );
}
                           
technique tColorStatic
{
    pass p0
    {
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
        ZWriteEnable    = True;
        ZEnable         = True;
        ZFunc           = LessEqual;
        
        FillMode        = <FillMode>;
        CullMode        = None;
        
        VertexShader    = compile vs_1_1 vsMain(GEOMETRY_DEFAULT);
        PixelShader     = compile ps_1_1 psMain();
    }
}

technique tColorSkinned
{
    pass p0
    {
        VertexShader    = compile vs_1_1 vsMain(GEOMETRY_SKINNED);
        PixelShader     = compile ps_1_1 psMain();
    }
}
