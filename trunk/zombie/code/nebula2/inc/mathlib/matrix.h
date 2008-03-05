#ifndef N_MATRIX_H
#define N_MATRIX_H
//-------------------------------------------------------------------
//  CLASSES
//  matrix33    -- 3x3 matrix
//  matrix34    -- 3x4 matrix
//  matrix44    -- 4x4 matrix
//-------------------------------------------------------------------
#include "kernel/ntypes.h"

#ifndef __USE_SSE__
#include "mathlib/_matrix33.h"
#include "mathlib/_matrix44.h"
typedef _matrix33 matrix33;
typedef _matrix44 matrix44;
#else
#include "mathlib/_matrix33_sse.h"
#include "mathlib/_matrix44_sse.h"
typedef _matrix33_sse matrix33;
typedef _matrix44_sse matrix44;
#endif
#include "mathlib/_matrix44d.h"
typedef _matrix44d matrix44d;

const static matrix44 matrix44_identity(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
);

const static matrix44 scaleAndBiasTextureProjection(
    0.5f, 0.0f,  0.0f, 0.0f,
    0.0f, -0.5f, 0.0f, 0.0f,
    0.0f, 0.0f,  1.0f, 0.0f,
    0.5f, 0.5f,  0.0f, 1.0f
);

//-------------------------------------------------------------------
#endif
