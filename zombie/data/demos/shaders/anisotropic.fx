/**
    Anisotropic lighting
    From NVIDIA SDK _
*/
shared float4x4 ModelViewProjection;
shared float4x4 InvModel;
shared float4x4 Model;

shared float3   LightPos;
shared float3   EyePos;

int FillMode = 3;//=Solid

texture DiffMap0;

sampler LookupMap = sampler_state
{
    Texture   = <DiffMap0>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    //MipFilter = NONE;  // inherit setting from app
    AddressU  = MIRROR;
//    AddressV = ?
};

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal   : NORMAL;
};

struct VS_OUTPUT
{
    float4 vPosition  : POSITION;
    float2 vTexCoord0 : TEXCOORD0;
};

VS_OUTPUT main(const VS_INPUT IN)
{
    VS_OUTPUT OUT;

    float3 worldNormal = normalize(mul((float3x3)Model, IN.Normal));

    //build float4
    float4 tempPos;
    tempPos.xyz = IN.Position.xyz;
    tempPos.w   = 1.0;

    //compute worldspace position
    float3 worldSpacePos = mul((float4x3)Model, tempPos);
    
    //vector from vertex to eye, normalized
    float3 vertToEye = normalize(EyePos - worldSpacePos);

    //h = normalize(l + e)
    float3 LightVec = normalize(LightPos); //assume directional light
    float3 halfAngle = normalize(vertToEye + LightVec);

    OUT.vTexCoord0.x = max(dot(LightVec, worldNormal), 0.0);
    OUT.vTexCoord0.y = max(dot(halfAngle, worldNormal), 0.0);
    
    // transform into homogeneous-clip space
    OUT.vPosition = mul(tempPos, ModelViewProjection);

    return OUT;
}


technique AnisotropicLighting
{
    pass P0
    {
		ZEnable = true;
		ZWriteEnable = true;
		ZFunc = LessEqual;
		AlphaBlendEnable = false;

		CullMode = None;
		FillMode = <FillMode>;

        // Set texture & filtering modes for texture stage #0
        Sampler[0]  = (LookupMap);
        
        // Set up TSS stages to use the texture color 
        // ( in a convoluted way: color = (tex.rgb * tex.aaa)*4 )
        ColorOp[0]   = Modulate4X;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Texture | AlphaReplicate;
        ColorOp[1]   = Disable;
        
        // Render states:
        Lighting     = False;

        // Shaders
        VertexShader = compile vs_1_1 main();
        PixelShader  = NULL;  
    }
}
