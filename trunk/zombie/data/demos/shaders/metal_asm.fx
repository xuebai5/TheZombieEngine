#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;
shared float4x4 InvView;
shared float4x4 View;

shared float4x4 TextureTransform0;

shared float4 EyePos;
shared float4 LightPos;

float4 CommonConst = float4(0.f, .5f, 1.f, 2.f);

float4 MatDiffuse = float4(1.f, 1.f, 1.f, 1.f);
float4 MatSpecular = float4(1.f, 1.f, 1.f, 1.f);

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

VERTEXSHADER asm_metal_vs =
decl {}
asm
{
    vs.1.1
    
    dcl_position   v0
    dcl_normal     v1
    dcl_texcoord   v2
    dcl_tangent    v3
    dcl_binormal   v4
    
    // output vertex position
    m4x4 oPos, v0, c0
    
    // output texture coordinates
    mov oT0, v2
    
    // output normal vector
    mov oT2, v1
    
    // find the light position in object space
    mov  r0, c9
    m4x4 r1, r0, c5
    
    // find light vector
    sub  r1, r1, v0
    
    // normalize light vector
    dp3  r2.x, r1, r1
    rsq  r2.x, r2.x
    mul  r1.xyz, r1.xyz, r2.xxx
    
    // output light vector
    mov oT1, r1
    
    // find the eye position in object space
    mov  r0, c4
    m4x4 r2, r0, c5
    
    // find eye vector
    sub  r2, r2, v0
    
    // normalize eye vector
    dp3  r3.x, r2, r2
    rsq  r3.x, r3.x
    mul  r2.xyz, r2.xyz, r3.xxx
    
    // output eye vector
    mov oT4, r2
    
    // find half angle vector
    add r3, r1, r2
    mul r3, r3, c10.yyy
    
    // normalize half angle vector
    dp3  r4.x, r3, r3
    rsq  r4.x, r4.x
    mul  r3.xyz, r3, r4.xxx
    
    mov r3.xy, v1.xy
    
    // normalize half angle vector
    dp3  r4.x, r3, r3
    rsq  r4.x, r4.x
    mul  r3.xyz, r3, r4.xxx
    
    // output half angle vector vector
    mov oT3, r3
};

PIXELSHADER asm_metal_ps =
decl {}
asm
{
    ps.1.4
    
    def c0, 0.0f, 0.0f, 0.0f, 1.0f
    
    texcrd r0.rgb, t0  // env map
    texcrd r1.rgb, t1  // L
    texcrd r2.rgb, t2  // N
    texcrd r3.rgb, t3  // H
    texcrd r4.rgb, t4  // V
    
    // N.L
    dp3 r5, r2, r1
    
    // offset basemap
    mov r0.a, c0.a
    add r0, r0, r5
    
    // N.H
    dp3 r4.r, r2, r3
    
    // H.H (aka ||H||^2)
    dp3_sat r4.g, r3, r3
    mov r4.g, 1-r4.g
    
    phase
    
    texld  r1, r4       // N.H^k
    texld  r0, r0       // base
    
    mul r1.rgb, r1, r5
    mul r1.rgb, r1, c1
    
    mul r5.rgb, r5, c2
    mul r0.rgb, r5, r0
    
    add_sat r0, r0, r1
};
                           
technique t0
{
    pass p0
    {
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
        ZWriteEnable = True;
        ZEnable = True;
        ZFunc = LessEqual;
        
        FillMode = <FillMode>;
        CullMode = None;

        VertexShaderConstant[0] = <ModelViewProjection>;
        VertexShaderConstant[4] = <EyePos>;
        VertexShaderConstant[5] = <InvView>;
        VertexShaderconstant[9] = <LightPos>;
        VertexShaderConstant[10] = <CommonConst>;

        PixelShaderConstant[1] = <MatSpecular>;
        PixelShaderConstant[2] = <MatDiffuse>;

        VertexShader = <asm_metal_vs>;
        PixelShader = <asm_metal_ps>;
    }
}
