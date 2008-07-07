#line 1 "libzombie.fx"
#ifndef LIBZOMBIE_FX
#define LIBZOMBIE_FX
//------------------------------------------------------------------------------
//  lib/libzombie.fx
//
//  Zombie-specific function library
//
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

// geometry types
#define GEOMETRY_DEFAULT    0
#define GEOMETRY_SKINNED    1
#define GEOMETRY_SWING      2
#define GEOMETRY_INSTANCED  3

// light types
#define LIGHT_POINT     0
#define LIGHT_SPOT      1
#define LIGHT_DIR       2
#define LIGHT_BULB      3
#define LIGHT_CONE      4

//keep this always at the end-
#define LIGHT_NUMTYPES  5

//structure for animating ocean waves:
struct Wave
{
    float freq;     // 2*PI / wavelength
    float amp;      // amplitude
    float phase;    // speed * 2*PI / wavelength
    float2 dir;
};

#define NumWaves 2
Wave wave[NumWaves] =
{
    { 1.0, 1.0, 0.5, float2(-1, 0) },
    { 2.0, 0.5, 1.3, float2(-0.7, 0.7) }
};

//------------------------------------------------------------------------------
//  helper functions
//------------------------------------------------------------------------------
float evaluateWave(Wave w, float2 pos, float t)
{
  return w.amp * sin(dot(w.dir, pos) * w.freq + t * w.phase);
}

float evaluateWaveDeriv(Wave w, float2 pos, float t)
{
  return w.freq * w.amp * cos(dot(w.dir, pos) * w.freq + t * w.phase);
}

float evaluateWaveSharp(Wave w, float2 pos, float t, float k)
{
  return w.amp * pow(sin(dot(w.dir, pos) * w.freq + t * w.phase)* 0.5 + 0.5, k);
}

float evaluateWaveDerivSharp(Wave w, float2 pos, float t, float k)
{
  return k * w.freq * w.amp * pow(sin(dot(w.dir, pos) * w.freq + t * w.phase)* 0.5 + 0.5, k - 1) * cos(dot(w.dir, pos) * w.freq + t * w.phase);
}

//------------------------------------------------------------------------------
/**
    transformWave()
*/
float4
transformWave(in const float4 position,
              in const float3 color,
              in const float  frequency,
              in const float  amplitude,
              in const float  time,
              out float3 normal,
              out float3 tangent,
              out float3 binormal)
{
    float4 pos = position;
    
    wave[0].freq = frequency;
    wave[0].amp  = amplitude;
    wave[1].freq = frequency * 2.0;
    wave[1].amp  = amplitude * 0.5;

    // sum wave
    float ddx = 0.0;
    float ddy = 0.0;

    for (int i = 0; i < NumWaves; i++)
    {
	    pos.y += evaluateWave(wave[i], pos.xz, time) * color.y;

	    float deriv = evaluateWaveDeriv(wave[i], pos.xz, time);
	    ddx += deriv * wave[i].dir.x;
	    ddy += deriv * wave[i].dir.y;
	}

    // compute tangent basis
    binormal = float3(1, ddx, 0);
    tangent = float3(0, ddy, 1);
    normal = float3(-ddx, 1, -ddy);
    
    return pos;
}

//------------------------------------------------------------------------------
/**
    transformInstanced()
    
    transform position using packed instanced attributes:
    
    @param  position        position in model space
    @param  instPosition    xyz=instance position, w=scale
    @param  instRotation    xyz=instance rotation (euler)
*/
float4
transformInstanced(in const float3 position,
                   in const float4 instPosition,
                   in const float4 instRotation)
{
    float4 oPos = float4(position, 1.0f);
    
    float3 rotatedPos;
    //scale:
    oPos.xyz *= instPosition.w;
    float3 vcos = float3(cos(instRotation.x), cos(instRotation.y), cos(instRotation.z));
    float3 vsin = float3(sin(instRotation.x), sin(instRotation.y), sin(instRotation.z));
    //rotate_x:
    rotatedPos = oPos.xyz;
    oPos.y = rotatedPos.y * vcos.x - rotatedPos.z * vsin.x;
    oPos.z = rotatedPos.z * vcos.x + rotatedPos.y * vsin.x;
    //rotate_y:
    rotatedPos = oPos.xyz;
    oPos.x = rotatedPos.x * vcos.y + rotatedPos.z * vsin.y;
    oPos.z = rotatedPos.z * vcos.y - rotatedPos.x * vsin.y;
    //rotate_z:
    rotatedPos = oPos.xyz;
    oPos.x = rotatedPos.x * vcos.z - rotatedPos.y * vsin.z;
    oPos.y = rotatedPos.y * vcos.z + rotatedPos.x * vsin.z;
    //translate:   
    oPos += float4(instPosition.xyz, 0.0f);
    return oPos;
}

//------------------------------------------------------------------------------
/**
    transformInstancedNormal()
    
    transform normal using packed instanced attributes:
    
    @param  normal          normal or tangent in model space
    @param  instRotation    xyz=instance rotation (euler)
*/
float3
transformInstancedNormal(in const float3 normal,
                         in const float4 instRotation)
{
    float3 oNormal = normal;
    float3 rotatedNormal;
    float3 vcos = float3(cos(instRotation.x), cos(instRotation.y), cos(instRotation.z));
    float3 vsin = float3(sin(instRotation.x), sin(instRotation.y), sin(instRotation.z));
    //rotate_x:
    rotatedNormal = oNormal.xyz;
    oNormal.y = rotatedNormal.y * vcos.x - rotatedNormal.z * vsin.x;
    oNormal.z = rotatedNormal.z * vcos.x + rotatedNormal.y * vsin.x;
    //rotate_y:
    rotatedNormal = oNormal.xyz;
    oNormal.x = rotatedNormal.x * vcos.y + rotatedNormal.z * vsin.y;
    oNormal.z = rotatedNormal.z * vcos.y - rotatedNormal.x * vsin.y;
    //rotate_z:
    rotatedNormal = oNormal.xyz;
    oNormal.x = rotatedNormal.x * vcos.z - rotatedNormal.y * vsin.z;
    oNormal.y = rotatedNormal.y * vcos.z + rotatedNormal.x * vsin.z;
    return oNormal;
}

//------------------------------------------------------------------------------
/**
	transformBillBoard()
	@param  pos is a centroid
	@param  uv  is a displacement of centroid
	@param  imv is a inverse model view
	@param  modelEyePos
*/
float4
transformBillBoard( const float4 pos,
                    const float2 uv,
                    const uniform float4x4 imv,
                    const uniform float3 modelEyePos )
{
    float3 VecY; 
    float3 VecZ;
    float3 VecX;
    float3 position = pos.xyz;
    VecY = mul(float3(0,1,0),imv); // Vertical axis of camera, this is invariant
    VecZ = position - modelEyePos; // vector to camera
    VecX = cross( VecZ, VecY );
    VecY = cross( VecX, VecZ );
    VecX = normalize( VecX );
    VecY = normalize( VecY );
    position += uv.x * VecX + uv.y * VecY ;
    return  float4(position , 1.0);
}

//------------------------------------------------------------------------------
/**
	swing2( )
	...
*/
float4
swing2( const float4 pos, const float3 weight, const uniform float time )
{
    return pos + float4 ( weight*float3( 0.03, 0.0, 0.03 ) * cos( 2*time + pos.y + pos.z  + pos.x ) , 0.0 );
}

//------------------------------------------------------------------------------
/**
*/
void
tangentSpaceLightHalfVector2(in const float3 pos,
                             in const float3 lightPos,
                             in const float3 eyePos,
                             in const float3 normal,
                             in const float3 binormal,
                             in const float3 tangent,
                             out float3 lightVec,
                             out float3 halfVec)
{
    // compute the light vector, eye vector and half vector in model space
    float3 lVec = normalize(lightPos - pos);
    float3 eVec = normalize(eyePos - pos);
    float3 hVec = normalize(lVec + eVec);

    // compute the binormal and tangent matrix
    float3x3 tangentMatrix = float3x3(tangent, binormal, normal);

    // transform with transpose of tangent matrix!
    lightVec = mul(tangentMatrix, lVec);
    halfVec  = mul(tangentMatrix, hVec);
}

//------------------------------------------------------------------------------
/**
*/
void
tangentSpaceLightHalfEyeVector2(in const float3 pos,
                                in const float3 lightPos,
                                in const float3 eyePos,
                                in const float3 normal,
                                in const float3 binormal,
                                in const float3 tangent,
                                out float3 lightVec,
                                out float3 halfVec,
                                out float3 eyeVec)
{
    // compute the light vector, eye vector and half vector in model space
    float3 lVec = normalize(lightPos - pos);
    float3 eVec = normalize(eyePos - pos);
    float3 hVec = normalize(lVec + eVec);

    // compute the binormal and tangent matrix
    float3x3 tangentMatrix = float3x3(tangent, binormal, normal);

    // transform with transpose of tangent matrix!
    lightVec = mul(tangentMatrix, lVec);
    halfVec  = mul(tangentMatrix, hVec);
    eyeVec   = mul(tangentMatrix, eVec);
}

//------------------------------------------------------------------------------
/**
*/
void
tangentSpaceLightHalfVectorDir(in const float3 pos,
                               in const float3 lightDir,
                               in const float3 eyePos,
                               in const float3 normal,
                               in const float3 binormal,
                               in const float3 tangent,
                               out float3 lightVec,
                               out float3 halfVec)
{
    // compute the light vector, eye vector and half vector in model space
    float3 lVec = normalize(lightDir);
    float3 eVec = normalize(eyePos - pos);
    float3 hVec = normalize(lVec + eVec);

    // compute the binormal and tangent matrix
    //float3 binormal = cross(normal, tangent);
    //float3 normal   = cross(tangent, binormal );
    float3x3 tangentMatrix = float3x3(tangent, binormal, normal);

    // transform with transpose of tangent matrix!
    lightVec = mul(tangentMatrix, lVec);
    halfVec  = mul(tangentMatrix, hVec);
}

//------------------------------------------------------------------------------
/**
*/
void
modelSpaceSurfaceNormalVector(in const float3 tangentNormal,
                              //in const float3 pos,
                              in const float3 normal,
                              in const float3 binormal,
                              in const float3 tangent,
                              out float3 modelNormal)
{
    float3x3 tangentMatrix = float3x3(tangent, binormal, normal);
    
    // transform with transpose of tangent matrix!
    modelNormal = mul(tangentNormal, tangentMatrix);
}

//------------------------------------------------------------------------------
/**
*/
void
modelSpaceLightHalfVector(in const float3 pos,
                          in const float3 lightPos,
                          in const float3 eyePos,
                          out float3 lightVec,
                          out float3 halfVec)
{
    float3 eyeVec;
    lightVec = normalize(lightPos - pos);
    eyeVec   = normalize(eyePos - pos);
    halfVec  = normalize(lightVec + eyeVec);
}

//------------------------------------------------------------------------------
/**
*/
void
modelSpaceLightHalfVectorDir(in const float3 pos,
                             in const float3 lightDir,
                             in const float3 eyePos,
                             out float3 lightVec,
                             out float3 halfVec)
{
    float3 eyeVec;
    lightVec = normalize(lightDir);
    eyeVec   = normalize(eyePos - pos);
    halfVec  = normalize(lightVec + eyeVec);
}

//------------------------------------------------------------------------------
/**
*/
void
vsLightmap(in const float4 position,
           in const float4 terrainCellMaxMinPos,
           in const float4x4 model,
           out float2 uvlight)
{
    float4 worldPos = mul(position, model);
    float4 localCellPos;
    localCellPos.x = terrainCellMaxMinPos.z - worldPos.x;
    localCellPos.y = worldPos.y;
    localCellPos.z = terrainCellMaxMinPos.w - worldPos.z;
    localCellPos.w = 1.0f;
    uvlight.x = 1.0f - localCellPos.x / (terrainCellMaxMinPos.z - terrainCellMaxMinPos.x);
    uvlight.y = 1.0f - localCellPos.z / (terrainCellMaxMinPos.w - terrainCellMaxMinPos.y);
}

float4 EncodeToRA(in float  val)
{
    float4 ret;
    val = val *65535;
    ret.r = floor(val / 256);
    ret.a = val - ret.r * 256;
    
    ret.ra /= 255.0; //
    
    ret.b = 0.0;
    ret.g = 0.0;
    return ret;    
}

float DecodeFromRA( in float4  val)
{
    float ret;
    val*= 255.0/65535.0;
    ret = val.r *256 + val.a;
    //ret /= 65535.0;
    return ret;    
}


float4 EncodeToRGBA_DXT5(in float  val)
{
	// Encode 14-bit values  to RGBA 5:6:5:8 format , this is the same format in dxt5
	// The best solution is use only 14 bit , the 6 MSb in the green chanel and  8  LSb in the alpha chanel. In aplha chanel's dxt5 has more definition of colors chanels and the LSb has more variation.
	// I've tested differents solution ;   5:6:5:0  ,  5:5:5:0,  4:0:4:8  ,  
	float4 ret;
	float D;
	float N = val * 16383;
	D = floor(N / 256.f);
	ret.a = (N - D * 256.f)/255.f;  // put in alpha the LSb becuase it has a maximun variation and alhas is higher quality
	
	//N = D;
	//D = floor(N / 16.f);
	//ret.b = (N - D * 16.f)/15.f;
	
	ret.g = D/63.f;
	ret.r = 1.f;  // The same as background, this is important for not lost quality.
	ret.b = 1.f;  // The same as background
	
	return ret;  
  
}

float DecodeFromRGBA_DXT5( in float4  val)
{
    //  16-bit values (RGB 5:6:5 format) 
	return dot( val.rgba , float4(                 0.f,									
								   256.f* 63.f/16383.f, // Convert 0..1 to 0..63  bits , displace  8 bits ( 256) , and convert to scale 0..1
								                   0.f, 
							             255.f/16383.f) // Convert 0..1 to 0..255 8 bits , displace 0 bits ( 1) , and convert to scale 0..1)
				);  
}

float4 EncodeToRGBA(in float  val)
{
    // The float has a 23 bit data , 1bit sign and 8 bits of mantisa then encode Only 3 chanels
    
    float c;
    float D;
    float m;
    float4 ret;
    
    D= val *( 4294967295); // 2^32 -1
    
    c = floor(D /256.f);
    m = fmod(D , 256.f);
    ret.a = m / 255.f;
    D=c;
    
    c = floor(D /256.f);
    m = fmod(D , 256.f);
    ret.b = m / 255.f;
    D=c;
    
    c = floor(D /256.f);
    m = fmod(D , 256.f);
    ret.g = m / 255.f;
    D=c;
    
    //c = floor(D /256.f);
    //m = fmod(D , 256.f);
    m = c;
    ret.r = m / 255.f;
    D=c;

	return ret;  
  
}

float DecodeFromRGBA( in float4  val)
{
    #define MAX_INT ( 4294967295) 
    return dot( val.rgba , float4(   255.f*(256.f*256.f*256.f/MAX_INT),									
                                     255.f*(      256.f*256.f/MAX_INT), // Convert 0..1 to 0..63  bits , displace  8 bits ( 256) , and convert to scale 0..1
                                     255.f*(           256.f/MAX_INT), 
                                     255.f                  /MAX_INT) // Convert 0..1 to 0..255 8 bits , displace 0 bits ( 1) , and convert to scale 0..1)
              );
}

float3 textureToNormal(float4 textureColor )
{
    textureColor   = 2.f * textureColor - 1.f;
#if 1 // Uncoment this line when use compressed normal map
    // The texture has encode the x in alpha chanel,
    textureColor.r = textureColor.a;
    textureColor.z = sqrt( 1 - dot(textureColor.rg, textureColor.rg) );    
    return textureColor.rgb;// no need normalize,
#else
    return normalize(textureColor.rgb);
#endif
}

//------------------------------------------------------------------------------
/**
    foamCurve()
    @param currentTime
    @param cicleTime, The curve's period
    @param climaxTime the time when has the maximun vulue
    @param endTime , the time when return 0 value
    @param smoth, if true use a smoth curve else use a peak
*/
float foamCurve(float currentTime, float cicleTime, float climaxTime, float endTime, const bool smoth)
{
    currentTime = fmod( currentTime,cicleTime);    
    float val;
    if ( currentTime <= climaxTime)
    {
        val = currentTime / climaxTime;
    }else if(currentTime <= endTime )
    {
        val =  1 - ( (currentTime - climaxTime ) /( endTime - climaxTime ) );
    } else
    {
        val = 0.0;
    }
    
    if ( smoth)
    {
        return sin( val * 0.5 * 3.1415926535897932384626433832795);
    } else
    {
        return val;
    }
}

#endif // LIBZOMBIE_FX
