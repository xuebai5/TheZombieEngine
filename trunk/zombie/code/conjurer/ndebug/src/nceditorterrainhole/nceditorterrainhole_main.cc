//------------------------------------------------------------------------------
//  nceditor_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchndebug.h"
#ifndef NGAME

#include "nceditorterrainhole/nceditorterrainhole.h"
#include "entity/nentityclassserver.h"
#include "entity/nentityobjectserver.h"
#include "nscene/nscenenode.h"
#include "nscene/ncsceneclass.h"
#include "util/nstring.h"
#include "nscene/nshapenode.h"
#include "nscene/ncsceneindexed.h"
#include "ngeomipmap/nterrainline.h"
#include "tools/nmeshbuilder.h"
#include "nphysics/ncphyterrain.h"
#include "nspatial/nspatialserver.h"
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatialquadtree.h"
#include "nasset/nentityassetbuilder.h"
#include "zombieentity/ncassetclass.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncEditorTerrainHole,nComponentObject);

//------------------------------------------------------------------------------
/**
*/
ncEditorTerrainHole::ncEditorTerrainHole()
{
}

//------------------------------------------------------------------------------
/**
*/
ncEditorTerrainHole::~ncEditorTerrainHole()
{
   
}

//------------------------------------------------------------------------------
/**
    @param loaded is false when the object is a new instance
*/
void
ncEditorTerrainHole::InitInstance(nObject::InitInstanceMsg /*initType*/)
{
}

//------------------------------------------------------------------------------
/**
    @param Line
*/
void
ncEditorTerrainHole::SetLine(nTerrainLine* line)
{
    if ( points.Size() > 0 )
    {
        nTerrainLine* oldLine = this->NewTerrainLine();
        this->RemoveHole(oldLine);
        n_delete(oldLine);
    }
    this->MakeHole(line);
}

//------------------------------------------------------------------------------
/**
    Recalculate the Hole , if the terrain's heightmap changes
*/
void 
ncEditorTerrainHole::ReCalculate(void)
{
    if ( points.Size() > 0 )
    {
        nTerrainLine* oldLine = this->NewTerrainLine();
        this->RemoveHole(oldLine);
        this->MakeHole(oldLine);
        n_delete(oldLine);
    }
}



//------------------------------------------------------------------------------
/**
    @param Line
*/
void
ncEditorTerrainHole::MakeHole(nTerrainLine* line)
{
    GetEntityClass()->SetObjectDirty();
    nEntityObject* hole = this->GetEntityObject();
   
    
    ncSceneClass* sceneClass = this->GetClassComponentSafe<ncSceneClass>();
    nString holeName;
    nString assetPath("wc:export/assets/"); 
    assetPath += GetEntityClass()->GetName();

    holeName.Format( "Hole_%.8X", hole->GetId() );
    nSceneNode* sceneNode = sceneClass ->GetRootNode();
    nShapeNode* sceneHole;

    nKernelServer::ks->PushCwd(sceneNode);
    sceneHole = static_cast<nShapeNode*>(nKernelServer::ks->New("nshapenode" , holeName.Get() )); // The Entity Class release this sceneHole
    nKernelServer::ks->PopCwd();
    hole->GetComponentSafe<ncSceneIndexed>()->SetSubscenePath( holeName.Get() );


    //Set Material
    sceneHole->SetDependency( "wc:export/assets/aDefaultTerrainHole/materials/Surface.n2", "/sys/servers/dependency/nsurfacedependencyserver", "setsurface" );

    //Set Mesh
    nString meshName(assetPath);
    meshName +=  "/meshes/";
    meshName += holeName;
    nMeshBuilder meshHole(1024,1024);
    line->GeneratePolygon(&meshHole , meshName);
    sceneHole->SetMesh( meshName.Get());

    nSpatialServer::Instance()->GetOutdoorEntity()->GetComponentSafe<ncPhyTerrain>()->AddHole( line );

    bbox3 box; // For spatial component
    box.begin_extend();
    for ( int idx = 0; idx < line->GetNumVertices() ; ++idx)
    {
        vector3 pos( line->GetPointPosition(idx) );
        points.Append( vector2( pos.x , pos.z) );
        box.extend(pos);
    }

    box.extend( box.center() + vector3( TINY , TINY , TINY) ); // Minimal box for physics and spatial prevent planes
    this->GetComponentSafe<ncSpatial>()->SetOriginalBBox( box );
}

//------------------------------------------------------------------------------
/**
    @param Line
*/
void
ncEditorTerrainHole::RemoveHole(nTerrainLine* /*line*/)
{
    ///@todo: Implement the removeHole
    //nSpatialServer::Instance()->GetOutdoorSpace()->GetComponentSafe<ncPhyTerrain>()->RemoveHole( line );

    // Not remove the mesh File 
    // Not remove scene
}

//------------------------------------------------------------------------------
/**
*/
nTerrainLine* 
ncEditorTerrainHole::NewTerrainLine()
{
    nTerrainLine* line = n_new(nTerrainLine);

    for ( int idx = 0; idx < points.Size() ; ++idx)
    {
        line->AddPoint(points[idx]);
    }

    return line;
}


#endif//!NGAME

//------------------------------------------------------------------------------
