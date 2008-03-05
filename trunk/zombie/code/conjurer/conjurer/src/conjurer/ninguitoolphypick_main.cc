#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
// ninguitoolplacer_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/ninguitoolphypick.h"
#include "entity/nentityobjectserver.h"
#include "nlevel/nlevel.h"
#include "nlevel/nlevelmanager.h"
#include "ngeomipmap/nfloatmap.h"
#include "nphysics/ncphycompositeobj.h"
#include "nphysics/nphycollide.h"

#include "conjurer/nobjecteditorstate.h"

nNebulaScriptClass(nInguiToolPhyPick, "ninguitool");

//------------------------------------------------------------------------------
/**
*/
nInguiToolPhyPick::nInguiToolPhyPick():
    entityInstance( 0 ),
    succesivePicking(false),
    isSticky(true)
{
    this->phyRay = static_cast<nPhyGeomRay*>( nKernelServer::Instance()->New("nphygeomray") );
    this->phyRay->SetCategories( nPhysicsGeom::Check );
    this->phyRay->SetCollidesWith( -1 );

    this->refObjState = static_cast<nObjectEditorState*>( nKernelServer::Instance()->Lookup("/app/conjurer/appstates/object") );
    n_assert( this->refObjState.isvalid() );

    this->contacts.SetFixedSize( 500 );
    this->sortedContacts.SetFixedSize( 500 );
}

//------------------------------------------------------------------------------
/**
*/
nInguiToolPhyPick::~nInguiToolPhyPick()
{
    this->phyRay->Release();
}

//------------------------------------------------------------------------------
/**
    @brief Do picking in a viewport.
    @param vp Viewport in wich the mouse pointer is located
    @param ray The ray to do the pick, in world space
    @return The ray parameter of picking position (for ordering intersections), or -1 if couldn't pick.
*/
float
nInguiToolPhyPick::Pick( nAppViewport* vp, vector2 /*mp*/, line3 ray)
{
    nPhysicsServer* phyServer = nPhysicsServer::Instance();
    this->phyRay->SetPosition( ray.b );
    this->phyRay->SetDirection( ray.m );
    this->phyRay->SetLength( ray.m.len() );

    int numCol = phyServer->Collide( phyRay, this->contacts.Size(), &contacts[0], true );
    float mint = -1.0f;
    if ( numCol > 0 )
    {
        // Get closest contact in the collision array or next object to previous selected
        int selContact = -1;
        this->sortedContacts.Reset();
        nEntityObject *lastSelectedEntity = 0;
        if ( this->refLastPickedEntity.isvalid() )
        {
            lastSelectedEntity = this->refLastPickedEntity;
        }

        vector3 ipos;
        for ( int colIndex = 0; colIndex < numCol; colIndex++ )
        {
            ncPhysicsObj* phyObj = this->contacts[ colIndex ].GetPhysicsObjA();
            if ( ! phyObj )
            {
                phyObj = this->contacts[ colIndex ].GetPhysicsObjB();
                if ( ! phyObj )
                {
                    continue;
                }
            }

            nEntityObject *obj = phyObj->GetEntityObject();
            n_assert( obj );
            if ( ! this->CanPickEntityObject(obj) )
            {
                continue;
            }

            this->contacts[ colIndex ].GetContactPosition( ipos );
            float t = ((ipos - ray.b).len()) / ray.m.len();

            // Get contact index, t, entity object and position in a struct to insert in array ordered by 't'
            SortedContact sortedContact;
            sortedContact.index = colIndex;
            sortedContact.t = t;
            sortedContact.pos = ipos;
            sortedContact.entity = obj;

            // Remember position of previous picked object if it's in the array
            if ( obj == lastSelectedEntity )
            {
                selContact = colIndex;
            }

            // Insert contact in ordered index nArray
            if ( colIndex > 0 )
            {
                bool inserted = false;
                for ( int colIndex2 = 0; colIndex2 < this->sortedContacts.Size(); colIndex2 ++ )
                {
                    SortedContact& sortedContact2 = this->sortedContacts.At( colIndex2 );
                    if ( sortedContact2.t > t )
                    {
                        // Insert in sorted contacts
                        this->sortedContacts.Insert( colIndex2, sortedContact );
                        inserted = true;
                        break;
                    }
                }
                if ( ! inserted )
                {
                    // Insert at end of sorted array
                    this->sortedContacts.Append( sortedContact );
                }
            }
            else
            {
                this->sortedContacts.Insert( 0, sortedContact );
            }
        }
        
        if ( this->sortedContacts.Empty() )
        {
            this->refLastPickedEntity = 0;
        }
        else
        {
            // Selected contact info
            SortedContact sortedContact;

            // Get sorted contact index
            int sortedContactIndex = -1;
            if ( selContact == -1 || ! this->succesivePicking )
            {
                // Select closest object
                sortedContactIndex = 0;
            }
            else
            {
                // Selected object is the next to the previously selected
                for ( int i = 0; i < this->sortedContacts.Size(); i++ )
                {
                    if ( this->sortedContacts[ i ].index == selContact )
                    {
                        sortedContactIndex = i;
                        break;
                    }
                }
                n_assert( sortedContactIndex != -1 );

                nEntityObject* curEntity = this->sortedContacts[ sortedContactIndex ].entity;
                int n = this->sortedContacts.Size();
                while ( curEntity == this->sortedContacts[ sortedContactIndex ].entity && n > 0 )
                {
                    sortedContactIndex = ( sortedContactIndex + 1 ) % this->sortedContacts.Size();
                    n--;
                }
            }

            // Get selected sorted contact struct
            sortedContact = this->sortedContacts[ sortedContactIndex ];

            // Get pick data from contact
            this->refLastPickedEntity = sortedContact.entity;
            n_assert( this->refLastPickedEntity.isvalid() );

            if ( this->state <= Inactive )
            {
                this->firstPos = sortedContact.pos;
                this->lastPos = this->firstPos;
            }
            else
            {
                this->lastPos = sortedContact.pos;
            }

            this->contacts[ sortedContact.index ].GetContactNormal( this->pickingNormal );

            mint = sortedContact.t;

            // Grid Snapping
            if ( this->refGrid.isvalid() )
            {
                this->firstPos = this->refGrid->SnapPosition( this->firstPos );
                this->lastPos = this->refGrid->SnapPosition( this->lastPos );
            }
        }
    }
    else
    {
        this->refLastPickedEntity = 0;
    }

    this->previousViewport = vp;

    return mint;
}

//------------------------------------------------------------------------------
/**
    @brief Return true if I can pick the given entity object
*/
bool
nInguiToolPhyPick::CanPickEntityObject(nEntityObject* entityObject)
{
    return true;
}

//------------------------------------------------------------------------------
/**
    @brief Set path for editor grid object
*/
void
nInguiToolPhyPick::SetGridPath( nString path )
{
    this->refGrid.set(path.Get());
}

//------------------------------------------------------------------------------
/**
    Get picked entity
*/
const
nEntityObject*
nInguiToolPhyPick::GetPickedEntity()
{
    return this->refLastPickedEntity;
}
//------------------------------------------------------------------------------
/**
    @brief Get created entity instance (for tools that create entities)
    @return Current entity instance while active, or 0 if not active
*/
nEntityObject* 
nInguiToolPhyPick::GetEntityInstance()
{
    if ( this->state > nInguiTool::Inactive )
    {
        return this->entityInstance;
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    @brief Set created entity instance
    @return Current entity instance while active, or 0 if not active
*/
void
nInguiToolPhyPick::SetEntityInstance(nEntityObject* instance)
{
    this->entityInstance = instance;
}
//------------------------------------------------------------------------------
