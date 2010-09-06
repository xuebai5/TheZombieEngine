#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;
shared float4x4 Model;
shared float4x4 View;
shared float4x4 ModelView;

shared float3 LightPos;
shared float3 EyePos;

float4 MatDiffuse;
float4 MatSpecular;

float Frequency; //noise_rate
float Noise;     //noise_scale

int FillMode = 3; //1=Point, 2=Wireframe, 3=Solid

texture NoiseMap0;

sampler2D NoiseSampler = sampler_state
{
    Texture = <NoiseMap0>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
}; 

struct VertexInput
{
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT;
};

struct VertexOutput
{
    float4 position : POSITION;
    float3 normal   : TEXCOORD0;
    float3 tangent  : TEXCOORD1;
    float3 binormal : TEXCOORD2;
    float3 viewVec  : TEXCOORD3;
    float3 lightVec : TEXCOORD4;
    float3 pos      : TEXCOORD5;
};

VertexOutput vsMain(const VertexInput IN)
{
    VertexOutput OUT = (VertexOutput) 0;
    
    float4 position = IN.position;
    OUT.position = mul(position, ModelViewProjection);

    OUT.normal = mul(IN.normal, Model).xyz;
    OUT.tangent = mul(IN.tangent, Model).xyz;
    float3 binormal = cross(IN.normal, IN.tangent);
    OUT.binormal = mul(binormal, Model).xyz;
    
    float3 worldPos = mul(position, Model).xyz;
    OUT.lightVec = (LightPos - worldPos);

    //OUT.viewVec = -mul(position, InvView);//eye vector in view space
    OUT.viewVec = EyePos - worldPos;
    OUT.pos = position.xyz * Frequency;

    return OUT;
}

float4 psMain(const VertexOutput IN) : COLOR
{
    float3 viewVec = normalize(IN.viewVec);
    float3 lightVec = normalize(IN.lightVec);

    float angle = Noise * (tex2D(NoiseSampler, IN.pos.xz).r - 0.5);
    float cosA, sinA;
    sincos(angle, sinA, cosA);

   float3 tangent = sinA * IN.tangent + cosA * IN.binormal;

    // Do the anisotropic lighting
    float diffuse = saturate(dot(lightVec, IN.normal));
    float cs = -dot(viewVec, tangent);
    float sn = sqrt(1 - cs * cs);
    float cl = dot(lightVec, tangent);
    float sl = sqrt(1 - cl * cl);
    float specular = pow(saturate(cs * cl + sn * sl), 32);

   // Output the results
   return diffuse * (MatDiffuse + specular * MatSpecular);
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
        PixelShader     = compile ps_2_0 psMain();
    }
}
