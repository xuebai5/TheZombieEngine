#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  ncspatialmc.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspatial/ncspatialmc.h"
#include "nspatial/nvisiblefrustumvisitor.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSpatialMC, ncSpatial);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSpatialMC)
    NSCRIPT_ADDCMD_COMPOBJECT('RACI', void, AddCellId, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RASI', void, AddSpaceId, 1, (nEntityObjectId), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    constructor
*/
ncSpatialMC::ncSpatialMC():
ncSpatial()
{
    this->cells    = n_new(nArray<ncSpatialCell*>(4, 4));
    this->cellIds  = n_new(nArray<int>(4, 4));
    this->spaceIds = n_new(nArray<nEntityObjectId>(4, 4));
    this->m_spatialComponentType = ncSpatial::N_SPATIAL_MC;
}

//------------------------------------------------------------------------------
/**
    destructor
*/
ncSpatialMC::~ncSpatialMC()
{
    n_delete(this->cells);
    n_delete(this->cellIds);
    n_delete(this->spaceIds);
}

//------------------------------------------------------------------------------
/**
    get space the entity is in (the one that contains the first cell in 
    the 'cells' array)
*/
ncSpatialSpace *
ncSpatialMC::GetSpace()
{
    if (this->cells->Empty())
    {
        return 0;
    }
    else
    {
        return this->cells->Front()->GetParentSpace();
    }
}

//------------------------------------------------------------------------------
/**
    get spaces the entity is in
*/
nArray<ncSpatialSpace*>
ncSpatialMC::GetSpaces() const
{
    nArray<ncSpatialSpace*> spaces(4, 4);
    for (int i = 0; i < this->cells->Size(); i++)
    {   
        spaces.Append(this->cells->At(i)->GetParentSpace());
    }

    return spaces;
}

//------------------------------------------------------------------------------
/**
    remove the entity from the spaces
*/
bool 
ncSpatialMC::RemoveFromSpaces()
{
    if (this->IsAssociatedToPortal())
    {
        return true;
    }

    ncSpatialCell *cell;
    while ( !this->cells->Empty() )
    {
        cell = this->cells->Back();
        n_assert_if(cell)
        {
            if ( !cell->RemoveEntity(this->GetEntityObject()) )
            {
                return false;
            }
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSpatialMC::SaveCmds(nPersistServer *ps)
{
    if (ncSpatial::SaveCmds(ps))
    {
        ncSpatialCell *currentCell = 0;
        for (int i = 0; i < this->cells->Size(); i++)
        {
            currentCell = this->cells->At(i);

            //--- addcellid ---
            if (!this->GetEntityObject()->IsA("nernswaypoint"))
            {
                int cellId = -1;
                int spaceId = -1;
                
                cellId = currentCell->GetId();
                if (currentCell->GetParentSpace() && currentCell->GetParentSpace()->GetEntityObject())
                {
                    spaceId = currentCell->GetParentSpace()->GetEntityObject()->GetId();
                }
                
                
                if (!ps->Put(this->GetEntityObject(), 'RACI', cellId))
                {
                    return false;
                }

                //--- addspaceid ---
                if (!ps->Put(this->GetEntityObject(), 'RASI', spaceId))
                {
                    return false;
                }
            }
        }

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    visibility visitor processing for an entity with a spatial multi cell component
*/
void 
ncSpatialMC::Accept(nVisibleFrustumVisitor &visitor)
{
    n_assert2(!this->cells->Empty(),
              "miquelangel.rujula: trying to accept a spatial mc component that has no cell!");
    
    ncSpatialCell *currentCell = 0;
    for (int i = 0; i < this->cells->Size(); ++i)
    {
        currentCell = this->cells->At(i);
        currentCell->AppendVisEntity(this->GetEntityObject());
    }

    this->SetFrameId(nSpatialServer::Instance()->GetFrameId());
    visitor.Visit(this->GetEntityObject());
}
