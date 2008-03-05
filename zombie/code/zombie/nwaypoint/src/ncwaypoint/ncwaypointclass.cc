//-----------------------------------------------------------------------------
//  ncwaypointclass.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnwaypoint.h"

#ifndef NGAME

#include "ncwaypoint/ncwaypointclass.h"
#include "ndebug/nceditorclass.h"

//-----------------------------------------------------------------------------
nNebulaComponentClass(ncWayPointClass,nComponentClass);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncWayPointClass)
NSCRIPT_INITCMDS_END()

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 15-Dec-2005   David Reyes    created
*/
ncWayPointClass::ncWayPointClass()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 15-Dec-2005   David Reyes    created
*/
ncWayPointClass::~ncWayPointClass()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    User init instance code.

    @param loaded indicates if the instance is bare new or loaded

    history:
        - 15-Dec-2005   David Reyes    created
*/
void ncWayPointClass::InitInstance(nObject::InitInstanceMsg)
{
    // assumes that the ncEditorClass it's already init
    ncEditorClass* editorclass( this->GetComponent<ncEditorClass>() );

    n_assert2( editorclass, "The ncEditorClass is required." );

    editorclass->SetAssetKeyInt( "LocalEntity", 1 );
}

#else

class nClassComponentClass * 
n_init_ncWayPointClass(char const *,class nComponentClassServer *)
{
    return 0;
}

#endif

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
