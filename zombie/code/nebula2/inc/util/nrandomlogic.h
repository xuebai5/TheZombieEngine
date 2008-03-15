#ifndef NRANDOMLOGIC_H
#define NRANDOMLOGIC_H
//------------------------------------------------------------------------------
/**
   @file nrandomlogic.h
   @author Luis Jose Cabellos Gomez
   
   (C) 2005 Conjurer Services, S.A.
*/

#include "mathlib/vector.h"

//------------------------------------------------------------------------------
///return a random integer between 0 and N_RAND_MAX
unsigned int n_rand_func();
/// set the seed of random functions 
void n_setseed( unsigned int seed );
/// get the actual seed of random functions
unsigned int n_getseed();
/// return a random integer between [0 .. max]
unsigned int n_rand_int( int max );
/// return a random integer between [min .. max]
unsigned int n_rand_int_in_range( int min, int max );
/// return a random float between [0 .. max]
float n_rand_real( float max );
/// return a random float between [min .. max]
float n_rand_real_in_range( float min, float max );
/// return a random float between [0 .. max] using a normal distribution
float n_rand_gaussian( float max );
/// return a random point within the boundaries of an axis aligned bounding box
vector3 n_rand_point_in_aabb( const vector3& min, const vector3& max );

#endif//NRANDOMLOGIC_H
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
