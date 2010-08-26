#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;

shared float4 ModelEyePos;
shared float4 ModelLightPos;

float4 MatDiffuse = float4( 1.f, 1.f, 1.f, 1.f );
float4 MatSpecular = float4( 1.f, 1.f, 1.f, 1.f );

int FillMode = 3; //1=Point, 2=Wireframe, 3=Solid
int CullMode = 2; //1=NoCull, 2=CW, 3=CCW

texture AmbientMap0; //environment_map
texture AmbientMap1; //NHHH_k256

sampler2D EnvironmentSampler = sampler_state
{
    Texture = <AmbientMap0>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
}; 

sampler2D NHk2Sampler = sampler_state
{
    Texture = <AmbientMap1>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
}; 


struct VsInput
{
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv0      : TEXCOORD0;
    float3 tangent  : TANGENT;
};

struct VsOutput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float3 lightVec : TEXCOORD1;
    float3 normal   : TEXCOORD2;
    float3 halfVec  : TEXCOORD3;
    float3 eyeVec   : TEXCOORD4;
};

VsOutput vsMain(const VsInput vsIn, const uniform int geomType)
{
    VsOutput vsOut;
    
    float4 pos = vsIn.position;
    
    vsOut.position = mul(pos, ModelViewProjection); //output vertex position
    vsOut.uv0 = vsIn.uv0;                           //output texture coordinates
    
    float3 normal = vsIn.normal;
    vsOut.normal = normal;                          //output normal vector
    
    float3 tangent = vsIn.tangent;
    float3 binormal = cross(vsIn.normal, vsIn.tangent);
    float3x3 tangentMatrix = float3x3(tangent, binormal, normal);

    float3 lightVec = ModelLightPos - pos;          //light vector
    float3 eyeVec = ModelEyePos - pos;              //eye vector
    vsOut.lightVec = normalize(lightVec);           //output normalized light vector
    vsOut.eyeVec = normalize(eyeVec);               //output normalized eye vector
    vsOut.halfVec = normalize(lightVec + eyeVec);
    
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
/*
texcrd r0.rgb, t0  // psIn.uv0
texcrd r1.rgb, t1  // psIn.lightVec
texcrd r2.rgb, t2  // psIn.normal
texcrd r3.rgb, t3  // H
texcrd r4.rgb, t4  // V
*/
    //N.L
    //dp3 r5, r2, r1
    float3 normal = normalize(psIn.normal);
    float3 light = normalize(psIn.lightVec);
    float NL = dot(normal, light);
    //offset basemap
    //mov r0.a, c0.a
    float4 uv0 = float4(psIn.uv0, 0.f, 1.0f);
    //add r0, r0, r5
    uv0.a = 1.0f;
    uv0 += NL;
    // N.H
    //dp3 r4.r, r2, r3
    float3 eyeVec = normalize(psIn.eyeVec);
    float3 halfVec = normalize(psIn.halfVec);
    eyeVec.r = dot(normal, halfVec);
    // H.H (aka ||H||^2)
    //dp3_sat r4.g, r3, r3
    eyeVec.g = saturate(dot(halfVec, halfVec));
    //mov r4.g, 1-r4.g
    eyeVec.g = 1.0f - eyeVec.g;
    
    //phase
    //texld  r1, r4       // N.H^k
    float4 specular = tex2D( NHk2Sampler, eyeVec.xy );
    //texld  r0, r0       // base
    float4 color = tex2D( EnvironmentSampler, uv0.xy );

    //mul r1.rgb, r1, r5
    specular.rgb = specular.rgb * NL;
    //mul r1.rgb, r1, c1
    specular.rgb = specular.rgb * MatSpecular;

    //mul r5.rgb, r5, c2
    float4 diffuse = NL * MatDiffuse;
    //mul r0.rgb, r5, r0
    color.rgb *= diffuse;

    //add_sat r0, r0, r1
    color = saturate(color + specular);
    return color;

    //return MatDiffuse;
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

        VertexShader    = compile vs_1_1 vsMain(GEOMETRY_DEFAULT);
        PixelShader     = compile ps_2_0 psMain();
    }
}
