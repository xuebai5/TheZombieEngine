#include "precompiled/pchndebug.h"
//------------------------------------------------------------------------------
//  ndebugcomponentserver.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "ndebug/ndebugcomponentserver.h"
#include "ndebug/nceditor.h"

#ifndef NGAME

nNebulaClass(nDebugComponentServer, "nroot");

nDebugComponentServer* nDebugComponentServer::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nDebugComponentServer::nDebugComponentServer() :
    drawFlags(ncEditor::None)
{
    n_assert(0 == Singleton);
    Singleton = this;
}
 
//------------------------------------------------------------------------------
/**
*/
nDebugComponentServer::~nDebugComponentServer()
{
    n_assert(0 != Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nDebugComponentServer::SetDrawingFlags(int flags)
{
    this->drawFlags = flags;
}

//------------------------------------------------------------------------------
/**
*/
int
nDebugComponentServer::GetDrawingFlags()
{
    return this->drawFlags;
}

//------------------------------------------------------------------------------
/**
    @param gfxServer graphics server
*/
#ifndef __ZOMBIE_EXPORTER__
void
nDebugComponentServer::Draw( nGfxServer2 * const gfxServer )
{
    for( int i = 0 ; i < this->objects.Size() ; ++i )
    {
        if( this->drawFlags )
        {
            this->objects[i]->SetDrawingFlags( this->drawFlags );
            this->objects[i]->Draw( gfxServer );
        }
    }
}
#endif
//------------------------------------------------------------------------------
/**
    @param object ncEditor to add
*/
void
nDebugComponentServer::Add( ncEditor * const object )
{
    n_assert( object );
    if( object )
    {
        this->objects.Append( object );
    }
}

//------------------------------------------------------------------------------
/**
    @param object ncEditor to remove
*/
void
nDebugComponentServer::Remove( ncEditor * const object )
{
    n_assert( object );
    if( object )
    {
        nArray<ncEditor*>::iterator i = this->objects.Find( object );
        if ( i != 0 )
        {
            this->objects.EraseQuick( i );
        }
    }
}

#else

// trick to avoid link error due to problem in python build system
// which includes nNebulaClass under NGAME protection
nClass* n_init_nDebugComponentServer(const char *, nKernelServer*) 
{
    return 0;
};

#endif//!NGAME

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
