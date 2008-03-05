//------------------------------------------------------------------------------
//  bill.fx
//
//  Support functions for Tragnarion shader library.
//
//  (C) 2003 Tragnarion Studios
//------------------------------------------------------------------------------

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
