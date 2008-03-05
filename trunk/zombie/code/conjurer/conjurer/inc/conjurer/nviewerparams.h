#ifndef N_VIEWERPARAMS_H
#define N_VIEWERPARAMS_H
//------------------------------------------------------------------------------
/**
    @file nviewerparams.h
    @class nViewerParams
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief Class that encapsulates parameters of a viewer, 
    ie. a point where an eye or a camera is placed.

    (C) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nroot.h"
#include "mathlib/polar.h"
//------------------------------------------------------------------------------
class nViewerParams
{
public:
    /// Constructor
    nViewerParams();

    /// Constructor
    nViewerParams(vector3 pos, polar2 ang);

    /// Destructor
    ~nViewerParams();

    /// operator ==
    bool operator==(const nViewerParams&);

    vector3 position;
    polar2 angles;

};

//------------------------------------------------------------------------------
/**
*/
inline
nViewerParams::nViewerParams():
    position(0.0f, 0.0f, 0.0f),
    angles(0.0f, 0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nViewerParams::nViewerParams(vector3 pos, polar2 ang):
    position(pos),
    angles(ang)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nViewerParams::~nViewerParams()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nViewerParams::operator==(const nViewerParams& viewerParams)
{
    return (this->position.isequal(viewerParams.position, 0.01f)) &&
        (this->angles.isequal(viewerParams.angles, 0.01f));
}

//------------------------------------------------------------------------------
#endif
