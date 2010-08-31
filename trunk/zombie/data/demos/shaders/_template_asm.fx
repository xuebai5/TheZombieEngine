#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;

float4 MatDiffuse = float4( 1.f, 0.f, 0.f, 1.f );

VERTEXSHADER asm_vs =
decl {}
asm
{
    dcl_position   v0

    m4x4 oPos, v0, c0

};

PIXELSHADER asm_ps =
decl {}
asm
{
    
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

        PixelShaderConstant[0] = <MatDiffuse>;

        VertexShader = <asm_vs>;
        PixelShader = <asm_ps>;
    }
}
