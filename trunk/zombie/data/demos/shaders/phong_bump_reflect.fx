shared float4x4 ModelViewProjection;
shared float4x4 Model;
shared float4x4 InvModel;

shared float3 EyePos;

int FillMode = 3;//=Solid

float3 LightPos;
float4 LightDiffuse;
float4 LightAmbient;

float MatSpecular;
float MatSpecularPower;
float BumpScale = 1.0; 
float MatLevel = 0.5;

texture DiffMap0 : DIFFUSE;

sampler2D ColorSampler = sampler_state {
    Texture = <DiffMap0>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
}; 

texture BumpMap0;

sampler2D NormalSampler = sampler_state
{
    Texture = <BumpMap0>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
}; 

texture CubeMap0 : ENVIRONMENT;

samplerCUBE EnvSampler = sampler_state
{
    Texture = <CubeMap0>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Clamp;
    AddressV = Clamp;
    AddressW = Clamp;
};

struct appdata
{
    float3 Position	: POSITION;
    float4 UV		: TEXCOORD0;
    float4 Normal	: NORMAL;
    float4 Tangent	: TANGENT0;
};

struct vertexOutput
{
    float4 HPosition    : POSITION;
    float2 UV		    : TEXCOORD0;
    float3 LightVec	    : TEXCOORD1;
    float3 WorldNormal	: TEXCOORD2;
    float3 WorldTangent	: TEXCOORD3;
    float3 WorldBinormal : TEXCOORD4;
    float3 WorldView	: TEXCOORD5;
};
 
///////// VERTEX SHADING /////////////////////

vertexOutput std_VS( appdata IN )
{
    vertexOutput OUT = (vertexOutput)0;

    OUT.WorldNormal = mul(IN.Normal, (matrix<float,3,3>) InvModel).xyz;
    OUT.WorldTangent = mul(IN.Tangent, (matrix<float,3,3>) InvModel).xyz;
    float3 binormal = cross(IN.Normal, IN.Tangent);
    OUT.WorldBinormal = mul( binormal, (matrix<float,3,3>) InvModel).xyz;
    
    float4 Po = float4(IN.Position.xyz, 1);
    float3 Pw = mul(Po, Model).xyz;
    OUT.LightVec = (LightPos - Pw);
    OUT.UV = IN.UV.xy;
    OUT.WorldView = normalize( EyePos - Pw );//normalize(InvView[3].xyz - Pw);
    OUT.HPosition = mul(Po, ModelViewProjection);
    return OUT;
}

///////// PIXEL SHADING //////////////////////

// Utility function for phong shading

void phong_shading( vertexOutput IN,
		            float3 LightColor,
		            float3 Nn,
		            float3 Ln,
		            float3 Vn,
		            out float3 DiffuseContrib,
		            out float3 SpecularContrib)
{
    float3 Hn = normalize(Vn + Ln);
    float4 litV = lit( dot(Ln,Nn), dot(Hn,Nn), MatSpecularPower );
    DiffuseContrib = litV.y * LightColor;
    SpecularContrib = litV.y * litV.z * MatSpecular * LightColor;
}

float4 std_PS( vertexOutput IN ) : COLOR
{
    float3 diffContrib;
    float3 specContrib;
    float3 Ln = normalize( IN.LightVec );
    float3 Vn = normalize( IN.WorldView );
    float3 Nn = normalize( IN.WorldNormal );
    float3 Tn = normalize( IN.WorldTangent );
    float3 Bn = normalize( IN.WorldBinormal );
    float3 bump = BumpScale * ( tex2D( NormalSampler, IN.UV ).rgb - float3(0.5,0.5,0.5) );
    Nn = Nn + bump.x * Tn + bump.y * Bn;
    Nn = normalize(Nn);
	phong_shading( IN, LightDiffuse, Nn, Ln, Vn, diffContrib, specContrib );

    float3 diffuseColor = tex2D( ColorSampler, IN.UV ).rgb;
    float3 result = specContrib + ( diffuseColor * ( diffContrib + LightAmbient ));
    float3 R = -reflect(Vn,Nn);
    float3 reflColor = MatLevel * texCUBE( EnvSampler, R.xyz ).rgb;
    result += diffuseColor * reflColor;
    return float4(result, 1);
}

technique Main
{
    pass p0
    {
		ZEnable = true;
		ZWriteEnable = true;
		ZFunc = LessEqual;
		AlphaBlendEnable = false;

		CullMode = None;
		FillMode = <FillMode>;

        VertexShader = compile vs_2_0 std_VS();
        PixelShader = compile ps_2_a std_PS();
    }
}
