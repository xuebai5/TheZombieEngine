#line 1 "wireframe.fx"
//------------------------------------------------------------------------------
//  ps2.0/wireframe.fx
//
//  Helper shader for drawing wireframe overlay.
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

#define GEOMETRY_DEFAULT    0
#define GEOMETRY_SKINNED    1
#define GEOMETRY_IMPOSTOR   2
#define GEOMETRY_OCEAN      3
#define GEOMETRY_PARTICLE2  4
#define GEOMETRY_PARTICLE   5

shared float4x4 ModelViewProjection;
shared float4x4 InvModelView;
shared float    Time;

float Frequency = 0.1f;
float Amplitude = 0.0f;

float Scale = 1.0f;

float4 MatDiffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);

int FillMode = 3; // solid

int CullMode = 2; /*CW*/

matrix<float,4,3> JointPalette[72];

struct VsInput
{
    float4 position : POSITION;
    float4 weights  : BLENDWEIGHT;
    float4 indices  : BLENDINDICES;
};

struct VsInputParticle
{
    float4 position  : POSITION;  // the particle position in world space
    float2 uv0       : TEXCOORD0; // the particle texture coordinates
    float2 extrude   : TEXCOORD1; // the particle corner offset
    float2 transform : TEXCOORD2; // the particle rotation and scale
    float4 color     : COLOR0;    // the particle color
};

struct vsInputParticle2Color
{
    float4 position : POSITION;    // the particle position in world space
    float3 velocity : NORMAL;      // the particle coded velocity
    float4 data     : COLOR0;      // the particle coded uv and corners,rotation and scale
};

struct VsInputImpostor
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float2 extrude  : TEXCOORD1; // the impostor corner offset
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
    else if (geomType == GEOMETRY_OCEAN)
    {
        wave[0].freq = Frequency;
        wave[0].amp  = Amplitude;
        wave[1].freq = Frequency * 2.0;
        wave[1].amp  = Amplitude * 0.5;

        for (int i = 0; i < NumWaves; i++)
        {
            position.y += evaluateWave(wave[i], position.xz, Time);
        }
    }

    vsOut.position = mul(position, ModelViewProjection);
    return vsOut;
}

VsOutput vsParticle(const VsInputParticle vsIn)
{
    float rotation = vsIn.transform[0];
    float size     = vsIn.transform[1];

    // build rotation matrix
    float sinAng, cosAng;
    sincos(rotation, sinAng, cosAng);
    float3x3 rot = {
        cosAng, -sinAng, 0.0f,
        sinAng,  cosAng, 0.0f,
        0.0f,    0.0f,   1.0f,
    };

    float4 position = vsIn.position;
    float3 extrude = float3(vsIn.extrude, 0.0f);
    extrude *= size;
    extrude = mul(rot, extrude);
    extrude = mul(extrude, (float3x3) InvModelView);
    position.xyz += extrude.xyz;

    VsOutput vsOut;
    vsOut.position = mul(position, ModelViewProjection);

    return vsOut;
}

VsOutput vsParticle2(const vsInputParticle2Color vsIn)
{
    float code     = vsIn.data[0];
    float rotation = vsIn.data[1];
    float size     = vsIn.data[2];
    float colorCode  = vsIn.data[3];

    // build rotation matrix
    float sinAng, cosAng;
    sincos(rotation, sinAng, cosAng);
    float3x3 rot = {
        cosAng, -sinAng, 0.0f,
        sinAng,  cosAng, 0.0f,
        0.0f,    0.0f,   1.0f,
    };

    // decode color data
    float4 rgba;
    rgba.z = modf(colorCode / 256.0f, colorCode);
    rgba.y = modf(colorCode / 256.0f, colorCode);
    rgba.x = modf(colorCode / 256.0f, colorCode);
    rgba.w = modf(code / 256.0f, code);
    rgba *= 256.0f / 255.0f;

    float4 position = vsIn.position;

    // the corner offset gets calculated from the velocity
    float3 extrude = mul(InvModelView, vsIn.velocity);
    if (code != 0.0f)
    {
        extrude = normalize(extrude);
        float vis = abs(extrude.z);
        size *= cos(vis * 3.14159f * 0.5f);
        rgba.w *= cos(vis * 3.14159f * 0.5f);
    };

    extrude.z = 0.0f;
    extrude = normalize(extrude);

    extrude *= size;
    extrude = mul(rot, extrude);
    extrude = mul(extrude, (float3x3) InvModelView);
    position.xyz += extrude.xyz;

    VsOutput vsOut;
    vsOut.position = mul(position, ModelViewProjection);

    return vsOut;
}

VsOutput vsImpostor(const VsInputImpostor vsIn)
{
    float4 position = vsIn.position;
    float3 extrude = float3(vsIn.extrude, 0.0f) * Scale;
    extrude = mul(extrude, (float3x3) InvModelView);
    position.xyz += extrude.xyz;
    VsOutput vsOut;
    vsOut.position = mul(position, ModelViewProjection);
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    return MatDiffuse;
}

VertexShader vsArray[6] = {compile vs_1_1 vsMain(GEOMETRY_DEFAULT),
                           compile vs_1_1 vsMain(GEOMETRY_SKINNED),
                           compile vs_1_1 vsImpostor(),
                           compile vs_1_1 vsMain(GEOMETRY_OCEAN),
                           compile vs_1_1 vsParticle2(),
                           compile vs_1_1 vsParticle()};
                           
int isSkinned = 0;

technique t0
{
    pass p0
    {
        ZWriteEnable     = False;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
        ZEnable          = True;
        ZFunc            = LessEqual;
        CullMode         = <CullMode>;
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        FogEnable        = False;
        FillMode         = <FillMode>;
        DepthBias        = -0.0001;

        VertexShader    = (vsArray[isSkinned]);//compile vs_1_1 vsMain();
        PixelShader     = compile ps_1_1 psMain();
    }
}
