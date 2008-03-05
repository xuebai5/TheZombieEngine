#ifndef NC_SPECIALFX_H
#define NC_SPECIALFX_H
//------------------------------------------------------------------------------
/**
    @class ncSpecialFX
    @ingroup SpecialFX
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief ...

    (C) 2006 Conjurer Services, S.A.
*/
#include "entity/nentity.h"

//------------------------------------------------------------------------------
class ncSpecialFX : public nComponentObject
{

    NCOMPONENT_DECLARE(ncSpecialFX,nComponentObject);

public:
    /// constructor
    ncSpecialFX();
    /// destructor
    ~ncSpecialFX();

    #ifndef NGAME
    /// dummy method to reset time offset for special fx
    void SetTimeOffset(bool);
    bool GetTimeOffset();
    #endif

    #ifndef NGAME
    #endif

};

//------------------------------------------------------------------------------
/**
*/

#endif /*NC_SPECIALFX_H*/
