//From the metal demo in RenderMonkey
//This effect implements simple hemispheric lighting by using the normal vector in view space
//to index a horizon gradient.
//30/08/2010 12:22:51 Not finished yet, missing the textures and integrating into ShadersApp

#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;

float4 CommonConst = float4(0.f, .5f, 1.f, 2.f);

float4 MatDiffuse = float4( 1.f, 0.f, 0.f, 1.f );


VERTEXSHADER asm_vs =
decl {}
asm
{
    vs.1.1
    
    dcl_position   v0
    dcl_normal     v1
    dcl_texcoord   v2
    dcl_tangent    v3
    dcl_binormal   v4
    
    m4x4  oPos, v0, c4              // OutPos = ObjSpacePos * WVP Matrix
    
    mov  r0, c2                     // Find the eye position in object space
    m4x4 r1, r0, c8
    
    sub   r8, r1, v0                // Compute view direction
    
    dp3   r10.x, r8, r8             // Normalize the view vector
    rsq   r10.y, r10.x              // 
    mul   r8, r8, r10.y             // 
    
    add   r1, r8, r8                // Compute 2V
    dp3   r10.x, r1, v1             // Compute 2V.N
    mad   r7, v1, r10.x, -r8        // Compute reflected view vector 
                     //     W = (2V.N)N - V
    
    mov   r5, v1                    // The normal vector
    mov   r4, v3                    // The tangent vector
    mul   r3, r4.yzxw, r5.zxyw      // Setup binormal (tX) vector perp to normal in r3
    mad   r3, r4.zxyw, -r5.yzxw, r3 // 
    m3x3  r6, r8, r3                // Transform view vector into tangent space
    mul   oT0, v2, c1.x             // Output bump map texture coordinates
    
    m3x3  oT1, r7, c12              // Output reflection vector (in object space)
    mov   oT2, r3                   // Output inverse tangent space basis vector 0 (X) 
    mov   oT3, r4                   // Output inverse tangent space basis vector 1 (Y) 
    mov   oT4, r5                   // Output inverse tangent space basis vector 2 (Z) 
    mov   oT5, r8                   // Output view vector in object space
    
    mov   oD0.xyz, r6               // Output view vector in tangent space
};

PIXELSHADER asm_ps =
decl {}
asm
{
    ps.1.4
    
    texld    r0,     t0                  // Read from microflake noise map to get perturbed normals
    texld    r1,     t1                  // Environment map reflection vector lookup ( for the gloss coat)
    texcrd   r3.rgb, t2                  // Inverse tangent space base vector 0 ( tan X )
    texcrd   r4.rgb, t3                  // Inverse tangent space base vector 1 ( tan Y )
    texcrd   r5.rgb, t4                  // Inverse tangent space base vector 2 ( ten Z )
    texcrd   r2.rgb, t5                  // The view vector 
    
    lrp      r0.rgb, c5.a, r0_bx2, c5    // Increase the microflake perturbed normal a bit
    
    mul      r3.rgb, r0.r, r3         
    mad      r3.rgb, r0.g, r4, r3
    mad      r3.rgb, r0.b, r5, r3        // Convert the perturbed normal from 
    dp3_sat  r4, r2, r3                  // Nbw.Vw
    
    mad      r3.rgb, r3_x2, r4, -r2      // Compute Rw = 2N(V.N)-V
    
    mul_x8   r1.rgb, r1, r1.a            // Brighten  up the luminosity by 8
    mov      r5, r1
    
    phase
    texld    r0, t0                      // Load the second microflake normal from the microflake                       
    texld    r1, r3                      // noise map to simulate second layer of metallic flakes
    
    mul_sat   r1.rgb, r1, r1.a           // Increase the 2nd microflake layer normal 
    +mad_sat  r0.a, 1-v0.b, 1-v0.b, c1.a // Compute (1-N.V)^2
    
    lrp       r4.rgb, c3, r4, v0.b       // Simulate color scattering
    mul       r4.rgb, r4_x2, c4          // Combine resulting color with base color
    mul       r1.rgb, r1, r0             // Add microflakes color 
    mad       r1.rgb, r1, r5, r4         // Combine sparkles with base color gradient
    lrp       r0.rgb, r0.a, r5, r1       // Add reflections from the environment map
    
    mul       r0.a, r1.a, c2.a          // Final color composite
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

        VertexShaderConstant[0] = <CommonConst>;
        VertexShaderConstant[4] = <ModelViewProjection>;

        PixelShaderConstant[0] = <MatDiffuse>;

        VertexShader = <asm_vs>;
        PixelShader = <asm_ps>;
    }
}
