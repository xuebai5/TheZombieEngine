#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  ncspatialindoorclass.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspatial/ncspatialindoorclass.h"
#include "zombieentity/ncloaderclass.h"

//------------------------------------------------------------------------------
nNebulaComponentClass(ncSpatialIndoorClass, nComponentClass);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSpatialIndoorClass)
    NSCRIPT_ADDCMD_COMPCLASS('RSOB', void, SetOriginalBBox, 6, (float, float, float, float, float, float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('RGOB', void, GetOriginalBBox, 0, (), 2, (vector3&, vector3&));
    NSCRIPT_ADDCMD_COMPCLASS('RSSN', void, SetShellName, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('RSHO', void, SetHasOccluders, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('RGHO', bool, GetHasOccluders, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    Initialize the component
*/
void 
ncSpatialIndoorClass::InitInstance(nObject::InitInstanceMsg /*initType*/)
{
    // check if this class has occluders
    ncLoaderClass *loaderClass = this->GetComponent<ncLoaderClass>();
    if (this->m_hasOccluders && loaderClass)
    {
        nString fileName = loaderClass->GetResourceFile();
        fileName.StripTrailingSlash();
        fileName.Append("/spatial/occluders.n2");

        n_assert(nFileServer2::Instance()->FileExists(fileName));
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSpatialIndoorClass::SaveCmds(nPersistServer *ps)
{
    //--- setoriginalbbox ---
    vector3 v0, v1;
    this->GetOriginalBBox(v0, v1);
    if (!ps->Put(this->GetEntityClass(), 'RSOB', v0.x, v0.y, v0.z, v1.x, v1.y, v1.z))
    {
        return false;
    }

    //--- setshellname ---
    if (!ps->Put(this->GetEntityClass(), 'RSSN', this->m_shellName.Get() ) )
    {
        return false;
    }
    
    //--- sethasoccluders ---
    if (!ps->Put(this->GetEntityClass(), 'RSHO', this->m_hasOccluders))
    {
        return false;
    }

    return true;
}

