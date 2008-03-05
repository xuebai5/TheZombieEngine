//-----------------------------------------------------------------------------
//  npreloadmanager_cmds.cc
//  (C) 2006 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchncommonapp.h"
#include "npreloadmanager/npreloadmanager.h"
#include "kernel/ncmdprotonativecpp.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nPreloadManager )

    NSCRIPT_ADDCMD('EBCP', void, BeginClassesToPreload, 0, (), 0, ());
    NSCRIPT_ADDCMD('EACP', void, AddClassToPreload , 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('EECP', void, EndClassesToPreload, 0, (), 0, ());
    NSCRIPT_ADDCMD('ENCP', int, GetNumClassesToPreload, 0, (), 0, ());
    NSCRIPT_ADDCMD('EGCP', const char*, GetClassToPreload, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ESCP', void, SetClassToPreload, 2, (int, const char*), 0, ());

    NSCRIPT_ADDCMD('EBSP', void, BeginSoundsToPreload, 0, (), 0, ());
    NSCRIPT_ADDCMD('EASP', void, AddSoundToPreload , 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('EESP', void, EndSoundsToPreload, 0, (), 0, ());
    NSCRIPT_ADDCMD('ENSP', int, GetNumSoundsToPreload, 0, (), 0, ());
    NSCRIPT_ADDCMD('EGSP', const char*, GetSoundToPreload, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ESSP', void, SetSoundToPreload, 2, (int, const char*), 0, ());

NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool
nPreloadManager::SaveCmds( nPersistServer* ps )
{
    // Class list
    ps->Put( this, 'EBCP' );
    for ( int i(0); i < this->classesToPreload.Size(); ++i )
    {
        ps->Put( this, 'EACP', this->classesToPreload[i].Get() );
    }
    ps->Put( this, 'EECP' );

    // Sound list
    ps->Put( this, 'EBSP' );
    for ( int i(0); i < this->soundsToPreload.Size(); ++i )
    {
        ps->Put( this, 'EASP', this->soundsToPreload[i].Get() );
    }
    ps->Put( this, 'EESP' );

    return true;
}
