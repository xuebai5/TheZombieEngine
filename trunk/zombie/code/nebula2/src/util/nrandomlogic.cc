#include "precompiled/pchnkernel.h"
/**
   @file nrandomlogic.cc
   @author Luis Jose Cabellos Gomez

   (C) 2005 Conjurer Services, S.A.
*/
#include "util/nrandomlogic.h"

static unsigned int n_randP = 0;
static const unsigned int N_RAND_MAX = 4194303; // 2^22 - 1

//------------------------------------------------------------------------------
/**
    @param seed the new seed
*/
void n_setseed( unsigned int seed )
{
    n_randP = seed & N_RAND_MAX;
}

//------------------------------------------------------------------------------
/**
    @returns the actual seed
*/
unsigned int n_getseed( )
{
    return n_randP;
}

//------------------------------------------------------------------------------
/**
    @return a random integer between 0 and N_RAND_MAX
*/
unsigned int n_rand_func() 
{
    unsigned int val;
    // n_randP = ( n_randP * 769 + 241 ) both numbers are prime numbers and result < UINT_MAX
    val = n_randP << 9;
    val += n_randP << 8;
    val += n_randP + 241;
    n_randP = val & N_RAND_MAX;
    return n_randP;
}

//------------------------------------------------------------------------------
/**
    @param max maximum value
    @returns a random integer between [0 .. max]
*/
unsigned int n_rand_int( int max ) 
{
    return ( ( max + 1 ) * n_rand_func() / ( N_RAND_MAX + 1 ) );
}

//------------------------------------------------------------------------------
/**
    Return a random integer between [min .. max]

    @param min minimum value
    @param max maximum value
    @return random integer between [min .. max]
*/
unsigned int n_rand_int_in_range( int min, int max )
{
    return ( (max - min + 1) * n_rand_func() / (N_RAND_MAX + 1)) + min;
}

//------------------------------------------------------------------------------
/**
    @param max maximum value
    @returns a random float between [0.0 .. max]
*/
float n_rand_real( float max )
{
    return ( max * n_rand_func() / N_RAND_MAX );
}

//------------------------------------------------------------------------------
/**
    Return a random float between [min .. max]

    @param min minimum value
    @param max maximum value
    @return random float between [min .. max]
*/
float n_rand_real_in_range( float min, float max )
{
    return ( (max-min) * n_rand_func() / N_RAND_MAX ) + min;
}

//------------------------------------------------------------------------------
/**
    @param max maximum value
    @returns a random float between [0.0 .. max]
*/
float n_rand_gaussian( float max )
{
    static bool isGenerated = false;
    float normal_x1;
    static float normal_x2 = 0.0f;

    float w;
    float value;

    if( isGenerated )
    {
        isGenerated = false;
        value = normal_x2;
    }
    else
    {
        do
        {
            normal_x1 = n_rand_real( 2.0f ) - 1.0f;
            normal_x2 = n_rand_real( 2.0f ) - 1.0f;
            w = normal_x1*normal_x1 + normal_x2*normal_x2;
        }while( w >= 1.0f || w < TINY );

        w = sqrt(log(w)*(-2.0f/w));

        normal_x1 *= w;
        normal_x2 *= w;

        isGenerated = true;
        value = normal_x1;
    }

    const float maxGaussian = 2.5f;

    // convert ot a [0.0f .. max] value
    value = fabs( value );
    if( value > maxGaussian )
    {
        value = maxGaussian;
    }
    value = max * value / maxGaussian;
    
    return value;
}

//------------------------------------------------------------------------------
/**
    Return a random point within the boundaries of an axis aligned bounding box

    @param min minimum vertex of the AABB
    @param max maximum vertes of the AABB
    @return random point within the AABB
*/
vector3 n_rand_point_in_aabb( const vector3& min, const vector3& max )
{
    return vector3( n_rand_real_in_range( min.x, max.x ),
                    n_rand_real_in_range( min.y, max.y ),
                    n_rand_real_in_range( min.z, max.z ) );
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
