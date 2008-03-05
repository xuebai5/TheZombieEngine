#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  ncspatialoccluder.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspatial/ncspatialoccluder.h"
#include "nspatial/nspatialvisitor.h"
#include "zombieentity/nctransform.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSpatialOccluder, ncSpatial);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSpatialOccluder)
    NSCRIPT_ADDCMD_COMPOBJECT('RSOV', void,  SetVertex, 2, (int, const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSDS', void,  SetDoubleSided, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGDS', bool,  GetDoubleSided, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RCAS', void,  ChangeActiveSide, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSID', void,  SetMinDist, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSMD', void,  SetMaxDist, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGID', float, GetMinDist, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGMD', float, GetMaxDist, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSOF', void,  SetFly, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGOF', bool,  GetFly, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
ncSpatialOccluder::ncSpatialOccluder():
ncSpatial(),
doubleSided(true),
minDist(-1.f),
maxDist(-1.f),
flies(false)
{
    // set spatial type
    this->m_spatialComponentType = ncSpatial::N_SPATIAL_OCCLUDER;

    // initialize vertices array
    this->vertices = n_new_array(vector3, 4);

    // initialize original vertices array
    this->originalVertices = n_new_array(vector3, 4);
}

//------------------------------------------------------------------------------
/**
*/
ncSpatialOccluder::~ncSpatialOccluder()
{
    n_delete_array(this->vertices);
    n_delete_array(this->originalVertices);
}

//------------------------------------------------------------------------------
/**
    Initialize the component
*/
void 
ncSpatialOccluder::InitInstance(nObject::InitInstanceMsg initType)
{
#ifndef NGAME  
    // hide the local occluders in the editor (the ones instanced from brushes and indoors)
    // the normal occluders must be editable
    if ( nEntityObjectServer::Normal != nEntityObjectServer::Instance()->GetEntityObjectType( this->GetEntityObject()->GetId() ))
    {
        this->GetEntityObject()->HideInEditor();  
    }
#endif

    this->spatialCat = nSpatialServer::Instance()->GetCategory(this->GetEntityObject());

#ifndef NGAME
    if( initType != nObject::ReloadedInstance )
    {
        // Register to EnterLimbo signal
        this->entityObject->BindSignal( NSIGNAL_OBJECT(nEntityObject, EnterLimbo), this, &ncSpatial::DoEnterLimbo, 0 );
    }
#endif

    if ( initType == nObject::NewInstance )
    {
        this->originalVertices[0].set( 1.f,  1.f,  0.f);
        this->originalVertices[1].set(-1.f,  1.f,  0.f);
        this->originalVertices[2].set(-1.f, -1.f,  0.f);
        this->originalVertices[3].set( 1.f, -1.f,  0.f);
        this->SetDetermineSpaceFlag(ncSpatial::DS_TRUE);
    }

#ifndef __ZOMBIE_EXPORTER__ // The exporter not has a spatial server 
                          // the iniInstace of this is called before initInstance ncTransform
    // transform the occluder
    this->Update(this->GetComponent<ncTransform>()->GetTransform());
#endif __ZOMBIE_EXPORTER
}

//------------------------------------------------------------------------------
/**
    update entity's position 
*/
void 
ncSpatialOccluder::Update(const matrix44 &worldMatrix)
{
    // update occluder vertices
    for (int i = 0; i < 4; i++)
    {
        this->vertices[i] = worldMatrix * this->originalVertices[i];
    }

    // recalculate occluder's center
    this->center.set(0.f, 0.f, 0.f);
    for (int i = 0; i < 4; i++)
    {
        this->center += this->vertices[i];
    }

    // calculate the average point, this is, the occluder's center
    this->center *= 0.25f;

    // recalculate occluder's plane
    this->p.set(this->vertices[1], this->vertices[2], this->vertices[0]);

    // recalculate occluder's area, because maybe it has been scaled
    vector3 base(this->vertices[1] - this->vertices[0]);
    vector3 height(this->vertices[2] - this->vertices[1]);
    this->area = base.len() * height.len();

    // update portal's bounding box
    this->UpdateBBox(worldMatrix);

    // resituate it in the corresponding cell
    nSpatialServer::Instance()->GetSpatialVisitor().Visit(this);
}

//------------------------------------------------------------------------------
/**
    update occluder's bbox from the current vertices
*/
void
ncSpatialOccluder::UpdateBBox(const matrix44 & /*worldMatrix*/)
{
    this->m_bbox.begin_extend();
    for (int i = 0; i < 4; i++)
    {
        this->m_bbox.extend(this->vertices[i]);
    }
}

//------------------------------------------------------------------------------
/**
    save state of the component
*/
bool 
ncSpatialOccluder::SaveCmds(nPersistServer *ps)
{
    if (!ncSpatial::SaveCmds(ps))
    {
        return false;
    }

    //--- setvertex ---
    for (int i = 0; i < 4; i++)
    {
        if (!ps->Put(this->GetEntityObject(), 'RSOV', i, this->originalVertices[i].x, 
                                                         this->originalVertices[i].y, 
                                                         this->originalVertices[i].z))
        {
            return false;
        }
    }

    //--- setdoublesided ---
    if (!ps->Put(this->GetEntityObject(), 'RSDS', this->doubleSided))
    {
        return false;
    }

    //--- setmindist ---
    if (!ps->Put(this->GetEntityObject(), 'RSID', this->GetMinDist()))
    {
        return false;
    }

    //--- setmaxdist ---
    if (!ps->Put(this->GetEntityObject(), 'RSMD', this->GetMaxDist()))
    {
        return false;
    }

    //--- setfly ---
    if (!ps->Put(this->GetEntityObject(), 'RSOF', this->GetFly()))
    {
        return false;
    }

    return true;
}

