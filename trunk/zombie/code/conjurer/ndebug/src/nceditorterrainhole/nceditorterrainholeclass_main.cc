//------------------------------------------------------------------------------
//  nceditorterrainholeclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchndebug.h"

#ifndef NGAME

#include "nceditorterrainhole/nceditorterrainholeclass.h"
#include "nceditorterrainhole/nceditorterrainhole.h"
#include "entity/nentityclassserver.h"
#include "entity/nentityobjectserver.h"
#include "nscene/nscenenode.h"
#include "nscene/ncsceneclass.h"
#include "util/nstring.h"
#include "nscene/nshapenode.h"
#include "nspatial/ncspatialquadtree.h"
#include "nasset/nentityassetbuilder.h"
#include "zombieentity/ncassetclass.h"

//------------------------------------------------------------------------------
nNebulaComponentClass(ncEditorTerrainHoleClass,nComponentClass);

//------------------------------------------------------------------------------
/**
*/
ncEditorTerrainHoleClass::ncEditorTerrainHoleClass()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncEditorTerrainHoleClass::~ncEditorTerrainHoleClass()
{
}

//------------------------------------------------------------------------------
/**
    @param loaded is false when the object is a new instance
*/
void
ncEditorTerrainHoleClass::InitInstance(nObject::InitInstanceMsg initType)
{
    // The iniInstance of editor components is called before others components
    if( initType != nObject::NewInstance )
    {
        // Empty
    } else
    {
        if ( ! GetEntityClass()->IsNative() )
        {
            /*
            // Create Asset
            ncAssetClass* asset = this->GetComponentSafe<ncAssetClass>();
            asset->SetDefaultResourceFile();
            asset->SetAssetEditable(true); // For save this after terrain
            nEntityAssetBuilder::MakeMeshPath( GetEntityClass() );
            nEntityAssetBuilder::MakeMaterialPath( GetEntityClass() );

            //Save scene node , load it in ncsene 
            nString nsceneNOH( nEntityAssetBuilder::GetSceneNOH( GetEntityClass() ) );
            nRoot*  sceneNode = nKernelServer::ks->New("nscenenode", nsceneNOH.Get());
            nEntityAssetBuilder::SaveSceneRoot( GetEntityClass() );
            sceneNode->Release();
            */
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param line the nTerrainline
*/
nEntityObject*
ncEditorTerrainHoleClass::CreateNewHole(nTerrainLine* line)
{
    nEntityObject* hole = nEntityObjectServer::Instance()->NewEntityObject( GetEntityClass()->GetName() );
    n_assert(this == hole->GetClassComponentSafe<ncEditorTerrainHoleClass>() )
    hole->GetComponentSafe<ncEditorTerrainHole>()->SetLine(line);
    return hole;
}
#endif//!NGAME

//------------------------------------------------------------------------------
