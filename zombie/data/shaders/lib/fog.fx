#line 1 "fog.fx"
//------------------------------------------------------------------------------
//  fog.fx
//
//  Fog functions for Tragnarion shader library
//
//  (C) 2004 Tragnarion Studios
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
	linearFog()

	calculate linear fog

    @param  position        a position in model space
	@param  fogDistances    x = start, y = end
	@param  modelView       the model*view matrix
	@return fog intensity
*/
float
linearFog( float3   position,
           float4   fogDistances,
           float4x4 modelView )
{
    float fogEnd        = fogDistances.x;
    float fogStart      = fogDistances.y;
    float4 camPos       = transformStatic( position, modelView ); // vertex position in camera space
    float fogRange      = fogEnd - fogStart;
    float vertexDist    = fogEnd + camPos.z;

    return clamp( ( vertexDist / fogRange ), 0.0f, 1.0f );
}

//------------------------------------------------------------------------------
/**
	expFog()

	calculate exponential fog

    @param  position        position in model space
	@param  fogDistances    x = density, y = fogend
	@param  modelView       the model*view matrix
	@return fog intensity
*/
float
expFog( float3   position,
        float4   fogDistances,
        float4x4 modelView )
{
    float4 camPos       = transformStatic( position, modelView ); // vertex position in camera space
    float fogDensity    = fogDistances.x;
    float fogEnd        = fogDistances.y;
    float vertexDist    = camPos.z / fogEnd * 4;

    return exp( vertexDist * fogDensity );
}

//------------------------------------------------------------------------------
/**
	exp2Fog()

	calculate exponential squared fog

    @param  position        a position in model space
	@param  fogDistances    x = density, y = fogend
	@param  modelEyePos     eye position in model space
	@param  modelView       the model*view matrix
	@return fog intensity
*/
float
exp2Fog( float3   position,
         float3   modelEyePos,
         float4   fogDistances,
         float4x4 modelView )
{
    float fogDensity    = fogDistances.x;
    float fogEnd        = fogDistances.y;
    float vertexDist    = distance( modelEyePos, position ) / fogEnd * 4;

    return exp( - (vertexDist * fogDensity) * ( vertexDist * fogDensity) );
}

//------------------------------------------------------------------------------
/**
	layeredFog()

	calculate layered fog

    @param  position        a position in model space
	@param  fogDistances    x = density, y = fogend
	@param  eyePos          eye position in world space
	@param  modelEyePos     eye position in model space
	@param  modelView       the model*view matrix
	@param  modelView       the model matrix
	@return fog intensity
*/
float
layeredFog( float4   position,
            float3   eyePos,
          //  float3   modelEyePos,
            float4   fogDistances,
            float4x4 Model //,
            //float4x4 modelView 
			)
{
    float fogTop        = fogDistances.x;
    float fogEnd        = fogDistances.y;
    float fogRange      = fogDistances.z;
   // float vertexDist    = distance( position, modelEyePos );    // distance to the viewer
    float4 worldPos = mul( position, Model );   // world position
    
    // project into XZ plane
    float4 cameraProj   = float4(eyePos,1.0f);
    cameraProj.y        = 0.0f;
    float4 WorldProj    = worldPos;
    WorldProj.y         = 0.0f;
    
    // scaled distance calculation in x-z plane
    float deltaD = distance( cameraProj, WorldProj ) / fogEnd * 2.0f;
    
    // height-based calculations
    float deltaY, densityIntegral;
    if( eyePos.y > fogTop )
    {
        if( worldPos.y < fogTop )
        {
            deltaY          = ( fogTop - worldPos.y ) / fogRange * 2;
            densityIntegral = abs( deltaY * deltaY * 0.5f );
        } else
        {
            deltaY          = 0.0f;
            densityIntegral = 0.0f;
        }
    }
    else
    {
        if( worldPos.y < fogTop )
        {
            float deltaA    = ( fogTop - eyePos.y ) / fogRange * 2;
            float deltaB    = ( fogTop - worldPos.y ) / fogRange * 2;
            deltaY          = abs( deltaA - deltaB );
            densityIntegral = abs( ( deltaA * deltaA * 0.5f ) - ( deltaB * deltaB * 0.5f ) );
        }
        else
        {
            deltaY          = abs( fogTop - eyePos.y ) / fogRange * 2;
            densityIntegral = abs( deltaY * deltaY * 0.5f );
        }
    }
    float density;
    if( deltaY != 0.0f )
    {
        density = ( sqrt( 1.0f + ( ( deltaD / deltaY ) * ( deltaD / deltaY ) ) ) ) * densityIntegral;
    }
    else
    {
        density = 0.0f;
    }
    return exp( -density );
}
