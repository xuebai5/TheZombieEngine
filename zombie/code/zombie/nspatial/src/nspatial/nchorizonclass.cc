/*-----------------------------------------------------------------------------
    @file nchorizonclass.cc
    @author Mateu Batle Sastre

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "precompiled/pchnspatial.h"
#include "nspatial/nchorizonclass.h"
#include "nspatial/nhorizonsegment.h"
#include "zombieentity/ncloaderclass.h"
#include "ngeomipmap/ncterraingmmclass.h"

//------------------------------------------------------------------------------
nNebulaComponentClass(ncHorizonClass, nComponentClass);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncHorizonClass)
    NSCRIPT_ADDCMD_COMPCLASS('BBHS', void, BeginHorizonSegments, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BAHS', void, AddHorizonSegment, 5, (int, int, bool, float, float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BEHS', void, EndHorizonSegments, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
*/
ncHorizonClass::ncHorizonClass() :
    horizonX(0),
    horizonZ(0),
    numSegmentsX(0),
    numSegmentsZ(0)
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
ncHorizonClass::~ncHorizonClass()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
ncHorizonClass::InitInstance(nObject::InitInstanceMsg /*initType*/)
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
bool 
ncHorizonClass::LoadResources()
{
    if (!this->horizonX || !this->horizonZ)
    {
        return this->LoadChunk();
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void 
ncHorizonClass::UnloadResources()
{
    if (this->horizonX)
    {
        n_delete_array(this->horizonX);
        this->horizonX = 0;
    }

    if (this->horizonZ)
    {
        n_delete_array(this->horizonZ);
        this->horizonZ = 0;
    }

    this->numSegmentsX = 0;
    this->numSegmentsZ = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool 
ncHorizonClass::IsValid()
{
    return (this->horizonX != 0 && this->horizonZ != 0);
}

//------------------------------------------------------------------------------
/**
*/
nString
ncHorizonClass::GetHorizonFilename() const
{
    nString path;

    const ncLoaderClass * loaderClass = this->GetComponent<ncLoaderClass>();
    n_assert(loaderClass);
    if( loaderClass->GetResourceFile() )
    {
        path.Append( loaderClass->GetResourceFile() );
    }
    path.Append( "/autogen/horizon.n2" );
    return path;
}

//------------------------------------------------------------------------------
/**
*/
void 
ncHorizonClass::SetSideSizeScale(float sidesize)
{
    n_assert(sidesize > 0.0);
    this->sidesize = sidesize;
}

//------------------------------------------------------------------------------
/**
*/
nHorizonSegment * 
ncHorizonClass::GetHorizonSegment(int bx, int bz, bool alongx) const
{
    if (alongx)
    {
        n_assert(bx >= 0 && bx < (this->numSegmentsX - 1));
        n_assert(bz >= 0 && bz < this->numSegmentsZ);
        return &this->horizonX[bz * (this->numSegmentsX - 1) + bx];
    }
    else
    {
        n_assert(bx >= 0 && bx < this->numSegmentsX);
        n_assert(bz >= 0 && bz < (this->numSegmentsZ - 1));
        return &this->horizonZ[bz * this->numSegmentsX + bx];
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncHorizonClass::BeginHorizonSegments(int bx, int bz)
{
    n_assert(!this->horizonX || !this->horizonZ);
    this->numSegmentsX = bx;
    this->numSegmentsZ = bz;
    this->horizonX = n_new_array(nHorizonSegment, (this->numSegmentsX - 1) * this->numSegmentsZ);
    n_assert(this->horizonX);
    this->horizonZ = n_new_array(nHorizonSegment, this->numSegmentsX * (this->numSegmentsZ - 1));
    n_assert(this->horizonZ);
}

//------------------------------------------------------------------------------
/**
*/
void
ncHorizonClass::EndHorizonSegments()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
void
ncHorizonClass::AddHorizonSegment(int bx, int bz, bool alongx, float y0, float y1)
{
    ncTerrainGMMClass * tgmmc = this->GetComponent<ncTerrainGMMClass>();
    if (tgmmc)
    {
        nHorizonSegment * hs = this->GetHorizonSegment(bx, bz, alongx);
        if (hs)
        {
            if (alongx)
            {
                hs->InitSegmentX(bx, bz, sidesize, y0, y1);                
            }
            else
            {
                hs->InitSegmentZ(bx, bz, sidesize, y0, y1);                
            }        
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ncHorizonClass::LoadChunk()
{
    nString path = this->GetHorizonFilename();

    if( nFileServer2::Instance()->FileExists( path.Get() ) )
    {
        nKernelServer::Instance()->PushCwd( this->GetEntityClass() );
        nKernelServer::Instance()->Load( path.Get(), false );
        nKernelServer::Instance()->PopCwd();
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
int 
ncHorizonClass::GetNumSegmentsX() const
{
    return this->numSegmentsX;
}

//------------------------------------------------------------------------------
/**
*/
int 
ncHorizonClass::GetNumSegmentsZ() const
{
    return this->numSegmentsZ;
}
