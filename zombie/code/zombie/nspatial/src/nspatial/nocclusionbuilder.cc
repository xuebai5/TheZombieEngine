#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  nocclusionbuilder.cc
//  (C) 2005 Conjurer Services, S.A.
//  @author Miquel Angel Rujula <>
//------------------------------------------------------------------------------

#include "nspatial/nocclusionbuilder.h"
#include "nspatial/ncspatialoccluder.h"
#include "entity/nobjectinstancer.h"
#include "entity/nentityobjectserver.h"

//------------------------------------------------------------------------------
/**
    constructor
*/
nOcclusionBuilder::nOcclusionBuilder()
{
    this->instancer = static_cast<nObjectInstancer*>(nKernelServer::Instance()->New("nobjectinstancer"));
    this->instancer->Init(16,16);
}

//------------------------------------------------------------------------------
/**
    destructor
*/
nOcclusionBuilder::~nOcclusionBuilder()
{
    this->instancer->Release();
}

//------------------------------------------------------------------------------
/**
    add an occluder
*/
void 
nOcclusionBuilder::AddOccluder(const vector3 *vertices, bool doubleSided)
{
    // create a new occluder
    nEntityObject *newOccluder = nEntityObjectServer::Instance()->NewEntityObject("neoccluder");

    // set the vertices to the new occluder
    ncSpatialOccluder *spatialOcc = newOccluder->GetComponentSafe<ncSpatialOccluder>();
    for (int i = 0; i < 4; i++)
    {
        spatialOcc->SetVertex(i, vertices[i]);
    }

    // set the double sided flag
    spatialOcc->SetDoubleSided(doubleSided);

    // add the new occluder to the instancer
    this->instancer->Append(newOccluder);
}

//------------------------------------------------------------------------------
/**
    end building and save instancer at the given asset path
*/
void 
nOcclusionBuilder::EndAndSave(const char *assetPath)
{
    nString fileName(assetPath);
    fileName.Append("/spatial");
    nFileServer2::Instance()->MakePath( fileName);
    fileName.Append("/occluders.n2");

    bool result = this->instancer->SaveAs(fileName.Get());
    
    if (!result)
    {
        n_message("Can't save occlusion instancer!");
    }
}
