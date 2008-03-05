#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  nspatialcellsaver.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "entity/nentityobjectserver.h"
#include "nspatial/nspatialcellsaver.h"
#include "nspatial/ncspatialcell.h"
#include "nspatial/nspatialtypes.h"

nNebulaScriptClass(nSpatialCellSaver, "nobject");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN(nSpatialCellSaver)
    NSCRIPT_ADDCMD('RAEI', void, AddEntityId, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD('RSWZ', void, SetWizard, 1, (int), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    constructor 1
*/
nSpatialCellSaver::nSpatialCellSaver():
m_cell(0)
{

}

//------------------------------------------------------------------------------
/**
    constructor 2
*/
nSpatialCellSaver::nSpatialCellSaver(ncSpatialCell *cell)
{
    this->m_cell = cell;
}

//------------------------------------------------------------------------------
/**
    destructor
*/
nSpatialCellSaver::~nSpatialCellSaver()
{

}

//------------------------------------------------------------------------------
/**
    save the entities contained in the cell
*/
bool 
nSpatialCellSaver::Save(const char *fileName)
{
    nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
    n_assert(ps);

    nArray< nArray<nEntityObjectId> > wizards;
    wizards.SetFixedSize( 0x100 );

    /// classify common entities
    const nArray<nEntityObject*> *categories = this->m_cell->GetCategories();
    for (int catIndex = 0; catIndex < nSpatialTypes::NUM_SPATIAL_CATEGORIES; catIndex++)
    {
        if (catIndex == nSpatialTypes::CAT_INDOOR_BRUSHES)
        {
            continue;
        }

        const nArray<nEntityObject*> &category = categories[catIndex];
        nEntityObject *entity = 0;
        for ( int i = 0; i < category.Size(); i++)
        {
            entity = category[i];

            if (entity->GetClass()->IsA("nemirageclass") ||
                nEntityObjectServer::Instance()->GetEntityObjectType(entity->GetId()) != nEntityObjectServer::Normal)

            {
                continue;
            }

            int index = (entity->GetId() & nEntityObjectServer::IDHIGHMASK ) >> 24 ;
            n_assert2(( index >= 0 ) && ( index < 0x100 ), "miquelangel.rujula: wrong wizard!");
            wizards[index].Append(entity->GetId());
        }
    }

    // save entities with cmd BeginNewObjectConfig cmd
    nCmd * cmd = ps->GetCmd(this, 'THIS');
    if (ps->BeginObjectWithCmd(this, cmd, fileName)) 
    {
        for( int i = 0 ; i < wizards.Size() ; ++i )
        {
            ps->Put(this, 'RSWZ', (i << 24) );

            for( int j = 0 ; j < wizards[i].Size() ; ++j )
            {
                ps->Put( this, 'RAEI', wizards[i][j] );
            }
        }

        ps->EndObject(true);
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    load the entities contained in the cell
*/
bool 
nSpatialCellSaver::Load(const char *fileName)
{
    // load parent classes if not already loaded
    nKernelServer::Instance()->PushCwd(this);
    nKernelServer::Instance()->Load(fileName, false);
    nKernelServer::Instance()->PopCwd();

    return true;
}

//------------------------------------------------------------------------------
/**
    add an entity id to the cell
*/
void
nSpatialCellSaver::AddEntityId(nEntityObjectId entityId)
{
    n_assert2(this->m_cell, 
              "miquelangel.rujula: trying to add an entity id to a cellsaver without cell!");

    this->m_cell->AddEntityId(entityId);
}

